#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <vector>
#include <mutex>
#include <atomic>
#include <string>
#include <functional>
#include "MemoryBlock.h"

class MemoryManager {
private:
    std::vector<MemoryBlock*> memoryBlocks;
    void* memoryPool;
    size_t poolSize;
    size_t usedMemory;
    mutable std::mutex mtx;
    std::string dumpFolder;
    std::atomic<bool> running{true};

    // Memory compaction/defragmentation support
    void compactMemory();

public:
    explicit MemoryManager(size_t sizeBytes, const std::string& dumpPath = "");
    ~MemoryManager();

    // Core memory operations
    int allocateMemory(size_t size);
    void deallocateMemory(int blockId);
    void* getBlockData(int blockId) const;
    bool setBlockData(int blockId, const void* data, size_t size);

    // Reference counting
    void increaseRefCount(int blockId);
    void decreaseRefCount(int blockId);
    int getRefCount(int blockId) const;

    // Garbage Collection
    void collectGarbage();
    void startBackgroundGC(unsigned int intervalSeconds = 30);
    void stopBackgroundGC();

    // Memory inspection and dumping
    std::string getMemoryDump() const;
    std::string getBlockInfo(int blockId) const;
    void dumpMemoryToFile() const;
    
    // Defragmentation control
    void setAutoDefrag(bool enable);
    bool needsDefragmentation() const;

    // Statistics
    size_t getTotalMemory() const { return poolSize; }
    size_t getUsedMemory() const { return usedMemory; }
    size_t getAvailableMemory() const { return poolSize - usedMemory; }
    size_t getBlockCount() const { return memoryBlocks.size(); }

    friend class GarbageCollector;
};

#endif // MEMORYMANAGER_H