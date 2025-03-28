#ifndef MPOINTER_H
#define MPOINTER_H

#include <string>
#include <iostream>
#include <stdexcept>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <memory>  // Para std::unique_ptr en caché local

template <typename T>
class MPointer {
private:
    static std::string serverUrl;
    int blockId;
    mutable std::unique_ptr<T> cachedValue;  // Cache local para reducir llamadas al servidor
    mutable bool cacheValid = false;

    // Método privado para actualizar la caché
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
        std::cout << "[DEBUG] Creating block of size: " << size << std::endl;
        
        nlohmann::json request = {
            {"size", size},
            {"type", typeid(T).name()},
            {"value", initialValue}  // Asegúrate de incluir el valor inicial
        };
        
        std::cout << "[DEBUG] Request JSON: " << request.dump() << std::endl;
        
        // Añade más detalles de depuración
        std::cout << "[DEBUG] Connecting to: " << serverUrl << std::endl;
        
        cpr::Response response;
        try {
            response = cpr::Post(
                cpr::Url{serverUrl + "/create"},
                cpr::Body{request.dump()},
                cpr::Header{
                    {"Content-Type", "application/json"},
                    {"Accept", "application/json"}
                },
                cpr::Timeout{5000},  // Aumenta el timeout
                cpr::Verbose{true}
            );
        } catch (const std::exception& e) {
            throw std::runtime_error("Connection error: " + std::string(e.what()));
        }
        
        std::cout << "[DEBUG] Response status: " << response.status_code << std::endl;
        std::cout << "[DEBUG] Response headers:\n";
        for (const auto& header : response.header) {
            std::cout << "  " << header.first << ": " << header.second << "\n";
        }
        std::cout << "[DEBUG] Response text: " << response.text << std::endl;
        
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
    // Operador * que devuelve referencia
    T& operator*() {
        if (!cacheValid || !cachedValue) {
            updateCache();
        }
        return *cachedValue;
    }

    // Versión const del operador *
    const T& operator*() const {
        if (!cacheValid || !cachedValue) {
            updateCache();
        }
        return *cachedValue;
    }

    // Operador de asignación desde valor
    MPointer<T>& operator=(const T& value) {
        nlohmann::json request = {{"value", value}};
        
        auto response = cpr::Put(
            cpr::Url{serverUrl + "/set/" + std::to_string(blockId)},
            cpr::Body{request.dump()},
            cpr::Header{{"Content-Type", "application/json"}},
            cpr::Timeout{1000}
        );
        
        if (response.status_code != 200) {
            throw std::runtime_error("Error al establecer valor: " + response.text);
        }
        
        // Actualizar caché local
        if (cachedValue) {
            *cachedValue = value;
        } else {
            cachedValue = std::make_unique<T>(value);
        }
        cacheValid = true;
        
        return *this;
    }

    // Operador de asignación desde otro MPointer
    MPointer<T>& operator=(const MPointer<T>& other) {
        if (this != &other) {
            // Liberar referencia actual
            if (blockId != -1) {
                cpr::Post(
                    cpr::Url{serverUrl + "/decref/" + std::to_string(blockId)},
                    cpr::Timeout{500}
                );
            }
            
            // Tomar nueva referencia
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
            
            // Invalidar caché
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
                
                if (response.status_code != 200) {
                    std::cerr << "Warning: Error al decrementar referencia para bloque " 
                              << blockId << ": " << response.text << std::endl;
                }
            } catch (...) {
                std::cerr << "Warning: Excepción en destructor de MPointer" << std::endl;
            }
        }
    }
};

template <typename T>
std::string MPointer<T>::serverUrl;

#endif // MPOINTER_H