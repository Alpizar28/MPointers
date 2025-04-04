#ifndef MEMORYBLOCK_H
#define MEMORYBLOCK_H

#include <cstdlib>
#include <cstring>  // Para memcpy

class MemoryBlock {
private:
    void* data;
    size_t size;
    int refCount;

public:
    MemoryBlock(size_t size);
    ~MemoryBlock();

    void* getData() const;
    size_t getSize() const;
    int getRefCount() const;
    bool setData(const void* newData, size_t dataSize);  // Añade este método
    void increaseRefCount();
    void decreaseRefCount();
};

#endif