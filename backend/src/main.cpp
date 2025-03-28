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

    // === Rutas ===

    CROW_ROUTE(app, "/create").methods("POST"_method)([&manager](const crow::request& req) {
        auto body = nlohmann::json::parse(req.body);
        size_t size = body["size"];
        std::string type = body.value("type", "int");

        int id = manager.allocateMemory(size);
        nlohmann::json response = {{"id", id}, {"type", type}};
        crow::response res{response.dump()};
        addCORSHeaders(res);
        return res;
    });

    CROW_ROUTE(app, "/set/<int>").methods("PUT"_method)([&manager](const crow::request& req, int id) {
        auto json = nlohmann::json::parse(req.body);
        int value = json["value"];
        bool ok = manager.setBlockData(id, &value, sizeof(int));
        crow::response res;
        res.code = ok ? 200 : 400;
        addCORSHeaders(res);
        return res;
    });

    CROW_ROUTE(app, "/get/<int>").methods("GET"_method)([&manager](const crow::request&, int id) {
        void* data = manager.getBlockData(id);
        int value = *static_cast<int*>(data);
        nlohmann::json json = {{"value", value}};
        crow::response res{json.dump()};
        addCORSHeaders(res);
        return res;
    });

    CROW_ROUTE(app, "/incref/<int>").methods("POST"_method)([&manager](const crow::request&, int id) {
        manager.increaseRefCount(id);
        crow::response res;
        res.code = 200;
        addCORSHeaders(res);
        return res;
    });

    CROW_ROUTE(app, "/decref/<int>").methods("POST"_method)([&manager](const crow::request&, int id) {
        manager.decreaseRefCount(id);
        crow::response res;
        res.code = 200;
        addCORSHeaders(res);
        return res;
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
