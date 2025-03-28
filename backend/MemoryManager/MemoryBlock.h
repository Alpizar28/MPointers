#ifndef MEMORYBLOCK_H
#define MEMORYBLOCK_H
#include <cstddef>

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

    void increaseRefCount();
    void decreaseRefCount();
};

#endif
