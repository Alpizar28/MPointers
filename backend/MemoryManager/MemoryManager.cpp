#include "MemoryManager.h"
#include <algorithm>  // Para std::find
#include <iostream>   // Para imprimir mensajes si es necesario

// Constructor de MemoryManager
MemoryManager::MemoryManager() {
    // Inicialización del gestor de memoria, si es necesario
}

// Destructor de MemoryManager
MemoryManager::~MemoryManager() {
    // Limpiar los bloques de memoria, si es necesario
    for (auto block : memoryBlocks) {
        delete block;  // Liberar cada bloque de memoria
    }
    memoryBlocks.clear();  // Limpiar la lista
}

// Método para asignar un bloque de memoria
MemoryBlock* MemoryManager::allocateMemory(size_t size) {
    MemoryBlock* block = new MemoryBlock(size);  // Crear un nuevo bloque de memoria
    memoryBlocks.push_back(block);  // Almacenar el bloque de memoria
    return block;
}

// Método para liberar un bloque de memoria
void MemoryManager::deallocateMemory(MemoryBlock* block) {
    auto it = std::find(memoryBlocks.begin(), memoryBlocks.end(), block);
    if (it != memoryBlocks.end()) {
        delete *it;  // Eliminar el bloque de memoria
        memoryBlocks.erase(it);  // Eliminarlo de la lista
    }
}

// Método para aumentar el conteo de referencias
void MemoryManager::increaseRefCount(MemoryBlock* block) {
    block->increaseRefCount();
}

// Método para disminuir el conteo de referencias
void MemoryManager::decreaseRefCount(MemoryBlock* block) {
    block->decreaseRefCount();
}
