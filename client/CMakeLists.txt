cmake_minimum_required(VERSION 3.10)
project(Client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(cpr REQUIRED)

add_executable(client src/main.cpp)

target_include_directories(client PRIVATE include)

target_link_libraries(client PRIVATE cpr::cpr)
