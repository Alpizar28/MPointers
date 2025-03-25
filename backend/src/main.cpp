#include "MemoryManager.h"
#include "../include/memory_server.h"
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Uso: " << argv[0] << " -port PUERTO -memsize TAMAÑO_MB -dumpFolder CARPETA_DUMP\n";
        return 1;
    }

    int port = std::stoi(argv[1]);
    size_t memsize = std::stoul(argv[2]) * 1024 * 1024;  // Convertir MB a bytes
    std::string dumpFolder = argv[3];

    // Crear el servidor REST
    std::string address = "http://0.0.0.0:" + std::to_string(port);
    MemoryServer server(memsize, address);
    
    std::cout << "Servidor REST listo en " << address << "\n";
    std::cout << "Memoria asignada: " << (memsize / (1024 * 1024)) << " MB\n";
    std::cout << "Dumps se guardarán en: " << dumpFolder << "\n";

    // Mantener el servidor en ejecución
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == "exit") break;
    }

    return 0;
}