#include "GarbageCollector.h"
#include "MemoryBlock.h"
#include "MemoryDumper.h"  // Añade esta línea

GarbageCollector::GarbageCollector(std::unordered_map<int, MemoryBlock*>& table, const std::string& path) 
    : memoryTable(table), dumpPath(path) {}

void GarbageCollector::collect() {
    for (auto it = memoryTable.begin(); it != memoryTable.end(); ) {
        if (it->second->getRefCount() <= 0) {
            MemoryDumper::dumpToFile(it->first, it->second, dumpPath);  // Usa dumpPath
            delete it->second;
            it = memoryTable.erase(it);
        } else {
            ++it;
        }
    }
}