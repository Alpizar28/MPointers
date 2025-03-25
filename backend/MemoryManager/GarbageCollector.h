#ifndef GARBAGECOLLECTOR_H
#define GARBAGECOLLECTOR_H

class MemoryManager;

class GarbageCollector {
public:
    static void collect(MemoryManager& manager);
};

#endif