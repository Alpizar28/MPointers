#include "MemoryManager.h"
#include "crow/app.h"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <iostream>

void addCORSHeaders(crow::response& res) {
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type");
}

int main(int argc, char* argv[]) {
    int port = 8080;
    size_t memsize = 100 * 1024 * 1024; // 100 MB
    std::string dumpFolder = "dumps";

    for (int i = 1; i < argc; i += 2) {
        std::string arg = argv[i];
        if (arg == "-port" && i + 1 < argc)
            port = std::stoi(argv[i + 1]);
        else if (arg == "-memsize" && i + 1 < argc)
            memsize = std::stoul(argv[i + 1]) * 1024 * 1024;
        else if (arg == "-dumpFolder" && i + 1 < argc)
            dumpFolder = argv[i + 1];
    }

    if (!std::filesystem::exists(dumpFolder)) {
        std::filesystem::create_directories(dumpFolder);
    }

    MemoryManager manager(memsize, dumpFolder);
    crow::SimpleApp app;

    // === RUTAS ===

    CROW_ROUTE(app, "/create").methods("POST"_method)([&manager](const crow::request& req) {
        try {
            std::clog << "\n[/create] Body recibido:\n" << req.body << std::endl;

            auto body = nlohmann::json::parse(req.body);
            size_t size = body["size"].get<size_t>();
            std::string type = body["type"].get<std::string>();
            auto value_json = body["value"];

            int id = manager.allocateMemory(size, type);
            std::clog << "[/create] type: " << type << ", size: " << size << ", id: " << id << std::endl;
            std::clog << "  ↳ value_json crudo: " << value_json << std::endl;

            if (type == "int") {
                if (!value_json.is_number_integer()) {
                    throw std::runtime_error("Expected int value, got: " + value_json.dump());
                }
                int value = value_json.get<int>();
                std::clog << "  ↳ Valor int escrito: " << value << std::endl;
                manager.setBlockData(id, &value, sizeof(int));
            } else if (type == "float") {
                if (!value_json.is_number_float() && !value_json.is_number()) {
                    throw std::runtime_error("Expected float value, got: " + value_json.dump());
                }
                float value = value_json.get<float>();
                std::clog << "  ↳ Valor float escrito: " << value << std::endl;
                manager.setBlockData(id, &value, sizeof(float));
            } else {
                throw std::runtime_error("Unsupported type: " + type);
            }

            nlohmann::json response = {{"id", id}};
            crow::response res{response.dump()};
            addCORSHeaders(res);
            return res;

        } catch (const std::exception& e) {
            std::clog << "[/create] ERROR: " << e.what() << std::endl;
            crow::response res{400};
            res.write(std::string("Error: ") + e.what());
            addCORSHeaders(res);
            return res;
        }
    });

    CROW_ROUTE(app, "/set/<int>").methods("PUT"_method)(
        [&manager](const crow::request& req, int id) {
            try {
                auto json = nlohmann::json::parse(req.body);
                int value = json["value"].get<int>();

                std::clog << "[/set] id: " << id << ", valor recibido: " << value << std::endl;

                bool ok = manager.setBlockData(id, &value, sizeof(int));
                crow::response res(ok ? 200 : 400);
                addCORSHeaders(res);
                return res;
            } catch (const std::exception& e) {
                crow::response res{400};
                res.write(std::string("Error: ") + e.what());
                addCORSHeaders(res);
                return res;
            }
        }
    );

    CROW_ROUTE(app, "/get/<int>").methods("GET"_method)([&manager](const crow::request&, int id) {
        try {
            void* data = manager.getBlockData(id);
            int value = *static_cast<int*>(data);
            std::clog << "[/get] id: " << id << " => valor: " << value << std::endl;

            nlohmann::json json = {{"value", value}};
            crow::response res{json.dump()};
            addCORSHeaders(res);
            return res;
        } catch (const std::exception& e) {
            crow::response res{400};
            res.write(std::string("Error: ") + e.what());
            addCORSHeaders(res);
            return res;
        }
    });

    CROW_ROUTE(app, "/incref/<int>").methods("POST"_method)([&manager](const crow::request&, int id) {
        manager.increaseRefCount(id);
        crow::response res{200};
        addCORSHeaders(res);
        return res;
    });

    CROW_ROUTE(app, "/decref/<int>").methods("POST"_method)([&manager](const crow::request&, int id) {
        manager.decreaseRefCount(id);
        crow::response res{200};
        addCORSHeaders(res);
        return res;
    });

    CROW_ROUTE(app, "/<string>").methods("OPTIONS"_method)([](const std::string&) {
        crow::response res{200};
        addCORSHeaders(res);
        return res;
    });

    std::cout << "🚀 Servidor Crow escuchando en http://127.0.0.1:" << port << std::endl;
    std::cout << "🧠 Memoria asignada: " << (memsize / (1024 * 1024)) << " MB\n";
    std::cout << "📁 Dumps se guardarán en: " << dumpFolder << std::endl;
    std::cout << "🟢 Presiona Ctrl+C para detener el servidor\n";

    app.port(port).multithreaded().run();
}
