#ifndef GARBAGE_COLLECTOR_H
#define GARBAGE_COLLECTOR_H

#include "MemoryManager.h"

class GarbageCollector {
public:
    // Método para ejecutar el proceso de recolección de basura
    static void collect(MemoryManager& manager);
};

#endif // GARBAGE_COLLECTOR_H
