#ifndef MEMORYBLOCK_H
#define MEMORYBLOCK_H

#include <cstddef>
#include <atomic>

class MemoryBlock {
private:
    void* data;
    size_t size;
    std::atomic<int> refCount;
    int id;
    static std::atomic<int> nextId;

public:
    MemoryBlock(size_t s, int blockId = 0);
    ~MemoryBlock();
    
    // Solo declaraciones (sin implementaciones)
    void setData(void* ptr);
    void* getData() const;
    size_t getSize() const;
    int getId() const;
    int getRefCount() const;
    void increaseRefCount();
    void decreaseRefCount();
};

#endif