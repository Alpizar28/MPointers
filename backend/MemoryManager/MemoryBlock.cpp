#include "MemoryBlock.h"
#include <iostream>

std::atomic<int> MemoryBlock::nextId(1);

MemoryBlock::MemoryBlock(size_t s, int blockId) : 
    size(s), 
    refCount(1), 
    id(blockId > 0 ? blockId : nextId++) {
    
    data = malloc(size);
    if (!data) {
        throw std::bad_alloc();
    }
    std::cout << "Bloque creado (ID: " << id << ", Size: " << size << " bytes)\n";
}

MemoryBlock::~MemoryBlock() {
    free(data);
    std::cout << "Bloque liberado (ID: " << id << ")\n";
}

// Implementaciones de los métodos
void MemoryBlock::setData(void* ptr) { data = ptr; }
void* MemoryBlock::getData() const { return data; }
size_t MemoryBlock::getSize() const { return size; }
int MemoryBlock::getId() const { return id; }
int MemoryBlock::getRefCount() const { return refCount.load(); }
void MemoryBlock::increaseRefCount() { ++refCount; }
void MemoryBlock::decreaseRefCount() { --refCount; }