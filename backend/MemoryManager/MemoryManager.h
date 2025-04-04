#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "MemoryBlock.h"
#include "GarbageCollector.h"
#include <unordered_map>
#include <string>
#include <mutex>  

class MemoryManager {
private:
    size_t totalMemory;
    std::string dumpPath;
    std::unordered_map<int, MemoryBlock*> memoryTable;
    GarbageCollector gc;
    int nextId = 0;
    std::mutex mtx;  

public:
    MemoryManager(size_t memorySize, const std::string& dumpPath);
    ~MemoryManager();

    int allocateMemory(size_t size);
    bool setBlockData(int id, void* data, size_t dataSize);
    void* getBlockData(int id);
    void increaseRefCount(int id);
    void decreaseRefCount(int id);
    int allocateMemory(size_t size, const std::string& type = "int");
    
};

#endif