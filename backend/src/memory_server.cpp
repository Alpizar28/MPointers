#include "../include/memory_server.h"
#include <sstream>
#include <filesystem>

MemoryServer::MemoryServer(size_t poolSize, const utility::string_t& url) : 
    manager(poolSize, "dumps"), listener(url) {

    listener.support(web::http::methods::POST, [this](web::http::http_request request) {
        auto path = request.relative_uri().path();
        if (path == "/create") {
            handle_create(request);
        } else if (path.find("/incref/") != std::string::npos) {
            handle_increase_ref(request);
        } else if (path.find("/decref/") != std::string::npos) {
            handle_decrease_ref(request);
        } else {
            request.reply(web::http::status_codes::NotFound);
        }
    });

    listener.support(web::http::methods::GET, std::bind(&MemoryServer::handle_get, this, std::placeholders::_1));
    listener.support(web::http::methods::PUT, std::bind(&MemoryServer::handle_set, this, std::placeholders::_1));

    try {
        listener.open().wait();
        std::wcout << U("Listening for requests at: ") << url.c_str() << std::endl;
    } catch (const std::exception &e) {
        std::wcout << e.what() << std::endl;
    }
}

MemoryServer::~MemoryServer() {
    listener.close().wait();
}

// Manejadores HTTP :

void MemoryServer::handle_create(web::http::http_request request) {
    request.extract_json().then([this, request](web::json::value request_json) mutable {
        auto response = web::json::value::object();
        try {
            size_t size = request_json[U("size")].as_integer();
            int blockId = manager.allocateMemory(size);
            response[U("id")] = web::json::value::number(blockId);
            request.reply(web::http::status_codes::OK, response);
        } catch (const std::exception &e) {
            response[U("error")] = web::json::value::string(U("Invalid request"));
            request.reply(web::http::status_codes::BadRequest, response);
        }
    });
}

void MemoryServer::handle_get(web::http::http_request request) {
    auto path = request.relative_uri().path();
    auto idStr = path.substr(path.find_last_of('/') + 1);
    int blockId = std::stoi(idStr);

    auto response = web::json::value::object();
    try {
        void* data = manager.getBlockData(blockId);
        int value = *static_cast<int*>(data);
        response[U("value")] = web::json::value::number(value);
        request.reply(web::http::status_codes::OK, response);
    } catch (const std::exception& e) {
        response[U("error")] = web::json::value::string(U("Block not found"));
        request.reply(web::http::status_codes::NotFound, response);
    }
}

void MemoryServer::handle_set(web::http::http_request request) {
    auto path = request.relative_uri().path();
    auto idStr = path.substr(path.find_last_of('/') + 1);
    int blockId = std::stoi(idStr);

    request.extract_json().then([this, request, blockId](web::json::value request_json) mutable {
        auto response = web::json::value::object();
        try {
            int value = request_json[U("value")].as_integer();
            if (manager.setBlockData(blockId, &value, sizeof(value))) {
                request.reply(web::http::status_codes::OK);
            } else {
                response[U("error")] = web::json::value::string(U("Failed to set data"));
                request.reply(web::http::status_codes::BadRequest, response);
            }
        } catch (const std::exception &e) {
            response[U("error")] = web::json::value::string(U("Invalid request"));
            request.reply(web::http::status_codes::BadRequest, response);
        }
    });
}

void MemoryServer::handle_increase_ref(web::http::http_request request) {
    auto path = request.relative_uri().path();
    auto idStr = path.substr(path.find_last_of('/') + 1);
    int blockId = std::stoi(idStr);

    try {
        manager.increaseRefCount(blockId);
        request.reply(web::http::status_codes::OK);
    } catch (const std::exception& e) {
        auto response = web::json::value::object();
        response[U("error")] = web::json::value::string(U("Block not found"));
        request.reply(web::http::status_codes::NotFound, response);
    }
}

void MemoryServer::handle_decrease_ref(web::http::http_request request) {
    auto path = request.relative_uri().path();
    auto idStr = path.substr(path.find_last_of('/') + 1);
    int blockId = std::stoi(idStr);

    try {
        manager.decreaseRefCount(blockId);
        request.reply(web::http::status_codes::OK);
    } catch (const std::exception& e) {
        auto response = web::json::value::object();
        response[U("error")] = web::json::value::string(U("Block not found"));
        request.reply(web::http::status_codes::NotFound, response);
    }
}
