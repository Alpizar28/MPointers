cmake_minimum_required(VERSION 3.10)
project(MemoryManagerServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Busca cpprestsdk y lo enlaza
find_package(cpprestsdk REQUIRED)
find_package(Threads REQUIRED)

# Incluye las rutas necesarias
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/MemoryManager
    /usr/include  # Asegura que encuentre cpprest/
)

# Archivos fuente
file(GLOB SOURCES
    "src/*.cpp"
    "MemoryManager/*.cpp"
)
add_executable(server 
    src/main.cpp 
    MemoryManager/MemoryManager.cpp
    MemoryManager/MemoryBlock.cpp
    src/memory_server.cpp
)

target_link_libraries(server 
    PRIVATE 
    cpprestsdk::cpprest 
    pthread
    ssl
    crypto
    stdc++fs
)
