#ifndef MPOINTER_H
#define MPOINTER_H

#include <string>
#include <iostream>
#include <stdexcept>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <memory>
#include <mutex>
#include <typeinfo>

template <typename T>
class MPointer {
private:
    static std::string serverUrl;
    static std::mutex serverMutex;
    int blockId;
    mutable std::unique_ptr<T> cachedValue;
    mutable bool cacheValid = false;
    mutable std::mutex cacheMutex;

    static std::string demangleTypeName(const std::type_info& ti) {
        if (ti == typeid(int)) return "int";
        else if (ti == typeid(float)) return "float";
        else if (ti == typeid(double)) return "double";
        else if (ti == typeid(char)) return "char";
        else if (ti == typeid(bool)) return "bool";
        else return "unsupported";
    }

    void updateCache() const {
        std::lock_guard<std::mutex> lock(cacheMutex);

        auto response = cpr::Get(
            cpr::Url{serverUrl + "/get/" + std::to_string(blockId)},
            cpr::Header{{"Content-Type", "application/json"}},
            cpr::Timeout{1000}
        );

        if (response.status_code != 200) {
            throw std::runtime_error("Server error: " + response.text);
        }

        try {
            auto json = nlohmann::json::parse(response.text);
            cachedValue = std::make_unique<T>(json["value"].get<T>());
            cacheValid = true;
        } catch (const nlohmann::json::exception& e) {
            throw std::runtime_error("JSON parse error: " + std::string(e.what()));
        }
    }

public:
    static void Init(const std::string& url) {
        serverUrl = url;
    }

    MPointer(int id = -1) : blockId(id) {}

    MPointer(const MPointer& other) : blockId(other.blockId) {
        if (blockId != -1) {
            std::lock_guard<std::mutex> lock(serverMutex);
            auto response = cpr::Post(
                cpr::Url{serverUrl + "/incref/" + std::to_string(blockId)},
                cpr::Header{{"Content-Type", "application/json"}},
                cpr::Timeout{1000}
            );

            if (response.status_code != 200) {
                throw std::runtime_error("Error incrementing reference");
            }
        }
    }

    static MPointer<T> New(const T& initialValue = T()) {
        nlohmann::json request = {
            {"size", sizeof(T)},
            {"type", demangleTypeName(typeid(T))},
            {"value", initialValue}
        };

        std::clog << "[MPointer::New] Enviando:\n" << request.dump(4) << std::endl;

        auto response = cpr::Post(
            cpr::Url{serverUrl + "/create"},
            cpr::Body{request.dump()},
            cpr::Header{{"Content-Type", "application/json"}},
            cpr::Timeout{5000}
        );

        std::clog << "[MPointer::New] Status code: " << response.status_code << std::endl;
        std::clog << "[MPointer::New] Respuesta: " << response.text << std::endl;

        if (response.status_code != 200) {
            throw std::runtime_error("Create failed: " + response.text);
        }

        try {
            auto json = nlohmann::json::parse(response.text);
            return MPointer<T>(json["id"].get<int>());
        } catch (const nlohmann::json::exception& e) {
            throw std::runtime_error("JSON error: " + std::string(e.what()));
        }
    }

    T& operator*() {
        std::lock_guard<std::mutex> lock(cacheMutex);
        if (!cacheValid || !cachedValue) {
            updateCache();
        }
        return *cachedValue;
    }

    const T& operator*() const {
        std::lock_guard<std::mutex> lock(cacheMutex);
        if (!cacheValid || !cachedValue) {
            updateCache();
        }
        return *cachedValue;
    }

    MPointer<T>& operator=(const T& value) {
        nlohmann::json request = {{"value", value}};

        {
            std::lock_guard<std::mutex> lock(serverMutex);
            auto response = cpr::Put(
                cpr::Url{serverUrl + "/set/" + std::to_string(blockId)},
                cpr::Body{request.dump()},
                cpr::Header{{"Content-Type", "application/json"}},
                cpr::Timeout{1000}
            );

            if (response.status_code != 200) {
                throw std::runtime_error("Set failed: " + response.text);
            }
        }

        std::lock_guard<std::mutex> lock(cacheMutex);
        cachedValue = std::make_unique<T>(value);
        cacheValid = true;
        return *this;
    }

    MPointer<T>& operator=(const MPointer<T>& other) {
        if (this != &other) {
            if (blockId != -1) {
                std::lock_guard<std::mutex> lock(serverMutex);
                auto response = cpr::Post(
                    cpr::Url{serverUrl + "/decref/" + std::to_string(blockId)},
                    cpr::Header{{"Content-Type", "application/json"}},
                    cpr::Timeout{500}
                );
            }

            blockId = other.blockId;
            if (blockId != -1) {
                std::lock_guard<std::mutex> lock(serverMutex);
                auto response = cpr::Post(
                    cpr::Url{serverUrl + "/incref/" + std::to_string(blockId)},
                    cpr::Header{{"Content-Type", "application/json"}},
                    cpr::Timeout{1000}
                );
                if (response.status_code != 200) {
                    throw std::runtime_error("Incref failed");
                }
            }

            std::lock_guard<std::mutex> lock(cacheMutex);
            cachedValue.reset();
            cacheValid = false;
        }
        return *this;
    }

    ~MPointer() {
        if (blockId != -1) {
            try {
                std::lock_guard<std::mutex> lock(serverMutex);
                auto response = cpr::Post(
                    cpr::Url{serverUrl + "/decref/" + std::to_string(blockId)},
                    cpr::Header{{"Content-Type", "application/json"}},
                    cpr::Timeout{500}
                );
            } catch (...) {
                // Silent fail
            }
        }
    }
};

template <typename T>
std::string MPointer<T>::serverUrl;

template <typename T>
std::mutex MPointer<T>::serverMutex;

#endif // MPOINTER_H
