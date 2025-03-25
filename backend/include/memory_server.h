#ifndef MEMORY_SERVER_H
#define MEMORY_SERVER_H

#include <cpprest/http_listener.h>
#include "MemoryManager.h"

class MemoryServer {
private:
    MemoryManager manager;
    web::http::experimental::listener::http_listener listener;

public:
    MemoryServer(size_t poolSize, const utility::string_t& url);
    ~MemoryServer(); 

    void handle_create(web::http::http_request request);
    void handle_get(web::http::http_request request);
    void handle_set(web::http::http_request request);
    void handle_increase_ref(web::http::http_request request);
    void handle_decrease_ref(web::http::http_request request);
};

#endif
