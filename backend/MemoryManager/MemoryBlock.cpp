#include "MemoryBlock.h"
#include <cstdlib>

MemoryBlock::MemoryBlock(size_t size) : size(size), refCount(1) {
    data = malloc(size);
}

MemoryBlock::~MemoryBlock() {
    free(data);
}

void* MemoryBlock::getData() const {
    return data;
}

size_t MemoryBlock::getSize() const {
    return size;
}

int MemoryBlock::getRefCount() const {
    return refCount;
}

void MemoryBlock::increaseRefCount() {
    refCount++;
}

void MemoryBlock::decreaseRefCount() {
    refCount--;
}
