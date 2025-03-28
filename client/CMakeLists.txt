cmake_minimum_required(VERSION 3.10)
project(Client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(cpr REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

# Configuración de includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Cliente principal
add_executable(client
    src/main.cpp
)

target_link_libraries(client
    PRIVATE
    cpr::cpr
    nlohmann_json::nlohmann_json
)