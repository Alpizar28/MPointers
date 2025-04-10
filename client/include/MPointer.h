#ifndef MPOINTER_H
#define MPOINTER_H

#include <string>
#include <iostream>
#include <stdexcept>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <memory>
#include <type_traits>

template <typename T>
class MPointer {
private:
    static std::string serverUrl;
    int blockId;
    mutable std::unique_ptr<T> cachedValue;
    mutable bool cacheValid = false;

    void updateCache() const {
        auto response = cpr::Get(
            cpr::Url{serverUrl + "/get/" + std::to_string(blockId)},
            cpr::Timeout{1000}
        );

        if (response.status_code != 200) {
            throw std::runtime_error("Error al obtener valor del servidor: " + response.text);
        }

        auto json = nlohmann::json::parse(response.text);
        cachedValue = std::make_unique<T>(json["value"].get<T>());
        cacheValid = true;
    }

    static std::string typeAsString() {
        if (std::is_same<T, int>::value) return "int";
        if (std::is_same<T, float>::value) return "float";
        return "unknown";
    }

public:
    static void Init(const std::string& url) {
        serverUrl = url;
    }

    MPointer(int id = -1) : blockId(id) {}

    MPointer(const MPointer& other) : blockId(other.blockId) {
        if (blockId != -1) {
            auto response = cpr::Post(
                cpr::Url{serverUrl + "/incref/" + std::to_string(blockId)},
                cpr::Timeout{1000}
            );

            if (response.status_code != 200) {
                throw std::runtime_error("Error al incrementar referencia");
            }
        }
    }

    static MPointer<T> New(size_t size = sizeof(T), const T& initialValue = T()) {
        nlohmann::json request = {
            {"size", size},
            {"type", typeAsString()},
            {"value", initialValue}
        };

        cpr::Response response;
        try {
            response = cpr::Post(
                cpr::Url{serverUrl + "/create"},
                cpr::Body{request.dump()},
                cpr::Header{
                    {"Content-Type", "application/json"},
                    {"Accept", "application/json"}
                },
                cpr::Timeout{5000},
                cpr::Verbose{false}
            );
        } catch (const std::exception& e) {
            throw std::runtime_error("Connection error: " + std::string(e.what()));
        }

        if (response.status_code != 200) {
            throw std::runtime_error("Failed to create block. Status: " + 
                                     std::to_string(response.status_code) + 
                                     "\nResponse: " + response.text);
        }

        try {
            auto json = nlohmann::json::parse(response.text);
            return MPointer<T>(json["id"].get<int>());
        } catch (const nlohmann::json::exception& e) {
            throw std::runtime_error("JSON parse error: " + std::string(e.what()));
        }
    }

    T& operator*() {
        if (!cacheValid || !cachedValue) {
            updateCache();
        }
        return *cachedValue;
    }

    const T& operator*() const {
        if (!cacheValid || !cachedValue) {
            updateCache();
        }
        return *cachedValue;
    }

    // ✅ Agregado: operador & que devuelve el ID del bloque remoto
    int operator&() const {
        return blockId;
    }

    MPointer<T>& operator=(const T& value) {
        nlohmann::json request = {{"value", value}};

        std::clog << "[CLIENT DEBUG] → PUT /set/" << blockId 
                  << " con valor: " << value << std::endl;

        auto response = cpr::Put(
            cpr::Url{serverUrl + "/set/" + std::to_string(blockId)},
            cpr::Body{request.dump()},
            cpr::Header{{"Content-Type", "application/json"}},
            cpr::Timeout{1000}
        );

        if (response.status_code != 200) {
            throw std::runtime_error("Error al establecer valor: " + response.text);
        }

        if (cachedValue) {
            *cachedValue = value;
        } else {
            cachedValue = std::make_unique<T>(value);
        }
        cacheValid = true;

        return *this;
    }

    MPointer<T>& operator=(const MPointer<T>& other) {
        if (this != std::addressof(other)) {
            // Liberar la referencia actual
            if (blockId != -1) {
                cpr::Post(
                    cpr::Url{serverUrl + "/decref/" + std::to_string(blockId)},
                    cpr::Timeout{500}
                );
            }
    
            // Tomar la nueva referencia
            blockId = other.blockId;
            if (blockId != -1) {
                auto response = cpr::Post(
                    cpr::Url{serverUrl + "/incref/" + std::to_string(blockId)},
                    cpr::Timeout{1000}
                );
    
                if (response.status_code != 200) {
                    throw std::runtime_error("Error al incrementar referencia");
                }
            }
    
            // Invalidar caché local
            cachedValue.reset();
            cacheValid = false;
        }
        return *this;
    }
    

    ~MPointer() {
        if (blockId != -1) {
            try {
                auto response = cpr::Post(
                    cpr::Url{serverUrl + "/decref/" + std::to_string(blockId)},
                    cpr::Timeout{500}
                );
            } catch (...) {
                // Silenciar errores en el destructor
            }
        }
    }
};

template <typename T>
std::string MPointer<T>::serverUrl;

#endif // MPOINTER_H
