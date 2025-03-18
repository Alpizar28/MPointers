cmake_minimum_required(VERSION 3.10)
project(mq_client)

set(CMAKE_CXX_STANDARD 17)

# Incluyendo el directorio donde se encuentra la librería CPR
include_directories(/usr/local/include)

# Agregando el ejecutable
add_executable(mq_client src/main.cpp)

# Vinculando las librerías necesarias
target_link_libraries(mq_client cpr curl)
