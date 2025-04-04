#include "MemoryBlock.h"
#include <cstdlib>
#include <cstring> 

MemoryBlock::MemoryBlock(size_t size) : size(size), refCount(1) {
    data = malloc(size);
    memset(data, 0, size);
}

MemoryBlock::~MemoryBlock() {
    free(data);
}

void* MemoryBlock::getData() const {
    return data;
}

bool MemoryBlock::setData(const void* newData, size_t dataSize) {
    if (dataSize > size) return false;
    memcpy(data, newData, dataSize);
    return true;
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
