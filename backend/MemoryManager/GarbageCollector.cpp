#include "GarbageCollector.h"
#include "MemoryManager.h"

void GarbageCollector::collect(MemoryManager& manager) {
    manager.collectGarbage();
}