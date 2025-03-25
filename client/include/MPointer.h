#ifndef MPOINTER_H
#define MPOINTER_H

#include <string>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

template <typename T>
class MPointer {
private:
    static std::string serverUrl;
    int blockId;

public:
    static void Init(const std::string& url) {
        serverUrl = url;
    }

    MPointer(int id = -1) : blockId(id) {}

    static MPointer<T> New(size_t size = sizeof(T)) {
        nlohmann::json request = {{"size", size}};
        auto response = cpr::Post(
            cpr::Url{serverUrl + "/create"},
            cpr::Body{request.dump()},
            cpr::Header{{"Content-Type", "application/json"}}
        );
        
        auto json = nlohmann::json::parse(response.text);
        return MPointer<T>(json["id"].get<int>());
    }

    T operator*() {
        auto response = cpr::Get(
            cpr::Url{serverUrl + "/get/" + std::to_string(blockId)}
        );
        auto json = nlohmann::json::parse(response.text);
        return json["value"].get<T>();
    }

    void operator=(const T& value) {
        nlohmann::json request = {{"value", value}};
        cpr::Put(
            cpr::Url{serverUrl + "/set/" + std::to_string(blockId)},
            cpr::Body{request.dump()},
            cpr::Header{{"Content-Type", "application/json"}}
        );
    }

    // Operador de asignación entre MPointer
    MPointer<T>& operator=(const MPointer<T>& other) {
        if (this != &other) {
            // Decrementar referencia actual
            if (blockId != -1) {
                cpr::Post(
                    cpr::Url{serverUrl + "/decref/" + std::to_string(blockId)}
                );
            }
            
            // Copiar ID y aumentar referencia
            blockId = other.blockId;
            cpr::Post(
                cpr::Url{serverUrl + "/incref/" + std::to_string(blockId)}
            );
        }
        return *this;
    }

    ~MPointer() {
        if (blockId != -1) {
            cpr::Post(
                cpr::Url{serverUrl + "/decref/" + std::to_string(blockId)}
            );
        }
    }
};

template <typename T>
std::string MPointer<T>::serverUrl;

#endif // MPOINTER_H