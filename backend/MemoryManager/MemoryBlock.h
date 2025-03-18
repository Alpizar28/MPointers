#ifndef MEMORYBLOCK_H
#define MEMORYBLOCK_H

#include <cstddef>  // Para size_t

class MemoryBlock {
private:
    void* data;  // Puntero a los datos
    size_t size; // Tamaño del bloque de memoria
    int refCount; // Contador de referencias

public:
    // Constructor
    MemoryBlock(size_t s);

    // Destructor
    ~MemoryBlock();

    // Método para obtener el tamaño del bloque de memoria
    size_t getSize() const;

    // Método para obtener la dirección de los datos
    void* getData();

    // Métodos para manejar el conteo de referencias
    void increaseRefCount();
    void decreaseRefCount();
    int getRefCount() const;
};

#endif // MEMORYBLOCK_H
