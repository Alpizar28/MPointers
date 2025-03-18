#include "MemoryBlock.h"
#include <cstdlib>  // Para malloc y free

// Constructor de MemoryBlock
MemoryBlock::MemoryBlock(size_t s) : size(s), refCount(1) {
    data = malloc(size);  // Reserva la memoria
}

// Destructor de MemoryBlock
MemoryBlock::~MemoryBlock() {
    free(data);  // Libera la memoria
}

// Método para obtener el tamaño del bloque de memoria
size_t MemoryBlock::getSize() const {
    return size;
}

// Método para obtener la dirección de los datos
void* MemoryBlock::getData() {
    return data;
}

// Aumenta el conteo de referencias
void MemoryBlock::increaseRefCount() {
    ++refCount;
}

// Disminuye el conteo de referencias
void MemoryBlock::decreaseRefCount() {
    --refCount;
}

// Obtiene el conteo de referencias
int MemoryBlock::getRefCount() const {
    return refCount;
}
