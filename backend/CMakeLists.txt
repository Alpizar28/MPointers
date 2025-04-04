cmake_minimum_required(VERSION 3.10)
project(MessageQueueBackend)

set(CMAKE_CXX_STANDARD 17)

add_executable(server
    src/main.cpp
    MemoryManager/MemoryManager.cpp
    MemoryManager/MemoryBlock.cpp
    MemoryManager/GarbageCollector.cpp
    MemoryManager/MemoryDumper.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/crow/include
    ${PROJECT_SOURCE_DIR}/MemoryManager
)

find_package(nlohmann_json REQUIRED)
target_link_libraries(server nlohmann_json::nlohmann_json pthread)
