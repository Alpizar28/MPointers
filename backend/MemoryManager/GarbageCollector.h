#ifndef GARBAGECOLLECTOR_H
#define GARBAGECOLLECTOR_H

#include <unordered_map>

class MemoryBlock;

class GarbageCollector {
private:
    std::unordered_map<int, MemoryBlock*>& memoryTable;

public:
    GarbageCollector(std::unordered_map<int, MemoryBlock*>& table);
    void collect();
};

#endif
