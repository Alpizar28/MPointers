#include "MemoryManager.h"
#include "MemoryDumper.h"  // Incluir el header de MemoryDumper
#include <cstring>
#include <stdexcept>
#include <filesystem>
#include <mutex>  // Añade esto
#include <iostream>

MemoryManager::MemoryManager(size_t memorySize, const std::string& dumpPath) 
    : totalMemory(memorySize), dumpPath(dumpPath), gc(memoryTable, dumpPath) 
{
    if(!std::filesystem::exists(dumpPath)) {
        std::filesystem::create_directories(dumpPath);
    }
}

MemoryManager::~MemoryManager() {
    for (auto& pair : memoryTable) {
        delete pair.second;
    }
}

int MemoryManager::allocateMemory(size_t size, const std::string& type) {
    if (size > totalMemory) {
        throw std::runtime_error("Not enough memory");
    }

    MemoryBlock* block = new MemoryBlock(size);
    int id = nextId++;
    memoryTable[id] = block;
    MemoryDumper::dumpToFile(id, block, dumpPath);
    return id;
}


bool MemoryManager::setBlockData(int id, void* data, size_t dataSize) {
    std::lock_guard<std::mutex> lock(mtx);

    auto it = memoryTable.find(id);
    if (it == memoryTable.end()) return false;

    if (dataSize > it->second->getSize()) return false;

    std::memcpy(it->second->getData(), data, dataSize);

    // 🔥 AGREGAR ESTO
    int debugValue = *static_cast<int*>(data);
    std::clog << "[MemoryManager::setBlockData] id: " << id << " ← valor: " << debugValue << std::endl;

    MemoryDumper::dumpToFile(id, it->second, dumpPath);
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
    
    // Opcional: Generar dump al aumentar referencias
    MemoryDumper::dumpToFile(id, it->second, dumpPath);
}

void MemoryManager::decreaseRefCount(int id) {
    auto it = memoryTable.find(id);
    if (it == memoryTable.end()) throw std::runtime_error("Block not found");

    it->second->decreaseRefCount();
    
    // Generar dump antes de posible garbage collection
    MemoryDumper::dumpToFile(id, it->second, dumpPath);
    gc.collect();
}