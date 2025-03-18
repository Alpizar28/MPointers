#include "MemoryManager.h"
#include "MemoryBlock.h"
#include "GarbageCollector.h"  // Asegúrate de que esta línea esté presente
#include <iostream>

int main() {
    MemoryManager manager;

    // Crear bloques de memoria
    MemoryBlock* block1 = manager.allocateMemory(100);
    MemoryBlock* block2 = manager.allocateMemory(200);

    // Aumentar el conteo de referencias de block1
    manager.increaseRefCount(block1);

    // Liberar una referencia a block1
    manager.decreaseRefCount(block1);

    // Ejecutar el Garbage CollectorAuto stash before revert of "I
    std::cout << "Garbage Collection completed." << std::endl;

    return 0;
}
