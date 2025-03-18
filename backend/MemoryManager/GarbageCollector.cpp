#include "GarbageCollector.h"
#include <iostream>

// Método para recolectar la basura (liberar bloques sin referencias)
void GarbageCollector::collect(MemoryManager& manager) {
    for (auto it = manager.memoryBlocks.begin(); it != manager.memoryBlocks.end(); ) {
        MemoryBlock* block = *it;
        
        // Comprobar si el bloque de memoria tiene referencias (refCount == 0)
        if (block->getRefCount() == 0) {
            std::cout << "Liberando bloque de memoria de " << block->getSize() << " bytes\n";
            delete block;  // Liberar la memoria
            it = manager.memoryBlocks.erase(it);  // Eliminarlo de la lista
        } else {
            ++it;
        }
    }
}
