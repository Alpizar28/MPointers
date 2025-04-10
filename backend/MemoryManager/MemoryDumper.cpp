#include "MemoryDumper.h"
#include "MemoryBlock.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <filesystem>

void MemoryDumper::dumpToFile(int id, MemoryBlock* block, const std::string& dumpPath) {
    // Obtener timestamp preciso
    const auto now = std::chrono::system_clock::now();
    const auto timestamp = std::chrono::system_clock::to_time_t(now);
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    // Crear nombre de archivo único
    std::ostringstream filename;
    filename << dumpPath << "/dump_" 
             << std::put_time(std::localtime(&timestamp), "%Y%m%d_%H%M%S_")
             << std::setfill('0') << std::setw(3) << ms.count() << ".log";

    // Crear archivo
    std::ofstream dumpFile(filename.str());
    if (!dumpFile) {
        std::cerr << "Error: No se pudo crear archivo de dump en " 
                  << filename.str() << std::endl;
        return;
    }

    // Escribir cabecera
    dumpFile << "=== MEMORY DUMP ===\n"
             << "Timestamp: " 
             << std::put_time(std::localtime(&timestamp), "%Y-%m-%d %H:%M:%S.")
             << std::setfill('0') << std::setw(3) << ms.count() << "\n"
             << "Block ID: " << id << "\n"
             << "Size: " << block->getSize() << " bytes\n"
             << "Ref Count: " << block->getRefCount() << "\n"
             << "Data: ";

    // Volcar contenido
    if (block->getSize() >= sizeof(int)) {
        const int* data = static_cast<int*>(block->getData());
        dumpFile << "0x" << std::hex << std::setfill('0') << std::setw(8) << *data
                 << " (Decimal: " << std::dec << *data << ")";
    } else {
        dumpFile << "[Block too small for int]";
    }

    // Cierre
    dumpFile << "\n=== END DUMP ===";

    std::clog << "Dump creado: " << filename.str() << std::endl;
}