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
    size_t memsize = 100 * 1024 * 1024;
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
            auto body = nlohmann::json::parse(req.body);
            size_t size = body.at("size").get<size_t>();
            auto value_json = body.at("value");

            int id = manager.allocateMemory(size, "int");
            int value = value_json.get<int>();
            manager.setBlockData(id, &value, sizeof(int));

            nlohmann::json response = {{"id", id}, {"type", "int"}};
            crow::response res{response.dump()};
            addCORSHeaders(res);
            return res;
        } catch (const std::exception& e) {
            return crow::response(400, std::string("Error en /create: ") + e.what());
        }
    });

    CROW_ROUTE(app, "/set/<int>").methods("PUT"_method)([&manager](const crow::request& req, int id) {
        try {
            auto json = nlohmann::json::parse(req.body);
            if (!json.contains("value")) return crow::response(400, "Falta el valor");

            int value = json["value"].get<int>();
            std::clog << "[SERVER DEBUG] /set/" << id << " recibido, valor: " << value << std::endl;
            bool ok = manager.setBlockData(id, &value, sizeof(int));
            return crow::response(ok ? 200 : 400);
        } catch (...) {
            return crow::response(500, "Error en /set");
        }
    });

    CROW_ROUTE(app, "/get/<int>").methods("GET"_method)([&manager](const crow::request&, int id) {
        try {
            void* data = manager.getBlockData(id);
            int value = *static_cast<int*>(data);
            nlohmann::json json = {{"value", value}};
            crow::response res{json.dump()};
            addCORSHeaders(res);
            return res;
        } catch (...) {
            return crow::response(404, "Bloque no encontrado");
        }
    });

    CROW_ROUTE(app, "/incref/<int>").methods("POST"_method)([&manager](const crow::request&, int id) {
        try {
            manager.increaseRefCount(id);
            crow::response res;
            res.code = 200;
            addCORSHeaders(res);
            return res;
        } catch (...) {
            return crow::response(404, "Bloque no encontrado");
        }
    });

    CROW_ROUTE(app, "/decref/<int>").methods("POST"_method)([&manager](const crow::request&, int id) {
        try {
            manager.decreaseRefCount(id);
            crow::response res;
            res.code = 200;
            addCORSHeaders(res);
            return res;
        } catch (...) {
            return crow::response(404, "Bloque no encontrado");
        }
    });

    // 🆕 Ruta para forzar dump manualmente desde el cliente
    CROW_ROUTE(app, "/forceDump/<int>").methods("POST"_method)([&manager](const crow::request&, int id) {
        try {
            std::clog << "[SERVER DEBUG] Forzando dump para ID: " << id << std::endl;
            manager.forceDump(id);  // método que vas a definir
            return crow::response(200, "Dump forzado para ID: " + std::to_string(id));
        } catch (...) {
            return crow::response(500, "Error al forzar dump");
        }
    });

    CROW_ROUTE(app, "/<string>").methods("OPTIONS"_method)([](const std::string&) {
        crow::response res;
        res.code = 200;
        addCORSHeaders(res);
        return res;
    });



    std::cout << "🚀 Servidor Crow escuchando en http://127.0.0.1:" << port << std::endl;
    std::cout << "🧠 Memoria asignada: " << (memsize / (1024 * 1024)) << " MB\n";
    std::cout << "📁 Dumps se guardarán en: " << dumpFolder << std::endl;
    std::cout << "🟢 Presiona Ctrl+C para detener el servidor\n";

    app.port(port).multithreaded().run();
}
