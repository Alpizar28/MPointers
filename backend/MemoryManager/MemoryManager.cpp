#include "MemoryManager.h"
#include <cstdlib>
#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <fstream>
#include <filesystem>


MemoryManager::MemoryManager(size_t sizeBytes, const std::string& dumpPath) : 
    poolSize(sizeBytes), usedMemory(0), dumpFolder(dumpPath) {
    memoryPool = malloc(poolSize);
    if (!memoryPool) {
        throw std::runtime_error("Failed to allocate memory pool");
    }
    
    // Crear directorio de dumps si no existe
    if (!dumpFolder.empty()) {
        std::filesystem::create_directories(dumpFolder);
    }
}

MemoryManager::~MemoryManager() {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto block : memoryBlocks) {
        delete block;
    }
    free(memoryPool);
}

int MemoryManager::allocateMemory(size_t size) {
    std::lock_guard<std::mutex> lock(mtx);
    
    if (usedMemory + size > poolSize) {
        throw std::runtime_error("Not enough memory in pool");
    }

    MemoryBlock* block = new MemoryBlock(size);
    block->setData(static_cast<char*>(memoryPool) + usedMemory);  // Usa setter
    usedMemory += size;
    
    memoryBlocks.push_back(block);
    return block->getId();
}

void* MemoryManager::getBlockData(int blockId) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = std::find_if(memoryBlocks.begin(), memoryBlocks.end(),
        [blockId](MemoryBlock* b) { return b->getId() == blockId; });
    
    if (it != memoryBlocks.end()) {
        return (*it)->getData();
    }
    throw std::runtime_error("Block not found");
}

bool MemoryManager::setBlockData(int blockId, const void* data, size_t size) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = std::find_if(memoryBlocks.begin(), memoryBlocks.end(),
        [blockId](MemoryBlock* b) { return b->getId() == blockId; });

    if (it != memoryBlocks.end() && (*it)->getSize() >= size) {
        std::memcpy((*it)->getData(), data, size);
        return true;
    }
    return false;
}

void MemoryManager::increaseRefCount(int blockId) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = std::find_if(memoryBlocks.begin(), memoryBlocks.end(),
        [blockId](MemoryBlock* b) { return b->getId() == blockId; });
    
    if (it != memoryBlocks.end()) {
        (*it)->increaseRefCount();
    } else {
        throw std::runtime_error("Block not found");
    }
}

void MemoryManager::decreaseRefCount(int blockId) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = std::find_if(memoryBlocks.begin(), memoryBlocks.end(),
        [blockId](MemoryBlock* b) { return b->getId() == blockId; });
    
    if (it != memoryBlocks.end()) {
        (*it)->decreaseRefCount();
        if ((*it)->getRefCount() <= 0) {
            collectGarbage();
        }
    }
}

void MemoryManager::collectGarbage() {
    memoryBlocks.erase(
        std::remove_if(memoryBlocks.begin(), memoryBlocks.end(),
            [](MemoryBlock* b) { 
                if (b->getRefCount() <= 0) {
                    delete b;
                    return true;
                }
                return false;
            }),
        memoryBlocks.end());
}