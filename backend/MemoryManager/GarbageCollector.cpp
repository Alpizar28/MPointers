#include "GarbageCollector.h"
#include "MemoryBlock.h"

GarbageCollector::GarbageCollector(std::unordered_map<int, MemoryBlock*>& table)
    : memoryTable(table) {}

void GarbageCollector::collect() {
    for (auto it = memoryTable.begin(); it != memoryTable.end(); ) {
        if (it->second->getRefCount() <= 0) {
            delete it->second;
            it = memoryTable.erase(it);
        } else {
            ++it;
        }
    }
}
