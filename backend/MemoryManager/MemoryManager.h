#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <vector>
#include "MemoryBlock.h"

class MemoryManager {
private:
    std::vector<MemoryBlock*> memoryBlocks;

public:
    MemoryManager();  // Constructor
    ~MemoryManager();  // Destructor

    MemoryBlock* allocateMemory(size_t size);
    void deallocateMemory(MemoryBlock* block);
    void increaseRefCount(MemoryBlock* block);
    void decreaseRefCount(MemoryBlock* block);

    friend class GarbageCollector;
};

#endif // MEMORYMANAGER_H
