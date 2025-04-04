#ifndef GARBAGECOLLECTOR_H
#define GARBAGECOLLECTOR_H

#include <unordered_map>
#include <string>

class MemoryBlock;

class GarbageCollector {
public:
    GarbageCollector(std::unordered_map<int, MemoryBlock*>& table, const std::string& dumpPath);
    void collect();

private:
    std::unordered_map<int, MemoryBlock*>& memoryTable;
    std::string dumpPath;  // Añade este miembro
};

#endif // GARBAGECOLLECTOR_H