#include "MemoryManager.h"
#include <cstring>
#include <stdexcept>

MemoryManager::MemoryManager(size_t memorySize, const std::string& dumpPath)
    : totalMemory(memorySize), dumpPath(dumpPath), gc(memoryTable) {}

MemoryManager::~MemoryManager() {
    for (auto& pair : memoryTable) {
        delete pair.second;
    }
}

int MemoryManager::allocateMemory(size_t size) {
    if (size > totalMemory) {
        throw std::runtime_error("Not enough memory");
    }

    MemoryBlock* block = new MemoryBlock(size);
    int id = nextId++;
    memoryTable[id] = block;
    return id;
}

bool MemoryManager::setBlockData(int id, void* data, size_t dataSize) {
    auto it = memoryTable.find(id);
    if (it == memoryTable.end()) return false;

    MemoryBlock* block = it->second;
    if (dataSize > block->getSize()) return false;

    std::memcpy(block->getData(), data, dataSize);
    return true;
}

void* MemoryManager::getBlockData(int id) {
    auto it = memoryTable.find(id);
    if (it == memoryTable.end()) throw std::runtime_error("Block not found");

    return it->second->getData();
}

void MemoryManager::increaseRefCount(int id) {
    auto it = memoryTable.find(id);
    if (it == memoryTable.end()) throw std::runtime_error("Block not found");

    it->second->increaseRefCount();
}

void MemoryManager::decreaseRefCount(int id) {
    auto it = memoryTable.find(id);
    if (it == memoryTable.end()) throw std::runtime_error("Block not found");

    it->second->decreaseRefCount();
    gc.collect();
}
