#ifndef MEMORYDUMPER_H
#define MEMORYDUMPER_H

#include "MemoryBlock.h"
#include <string>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <filesystem>

class MemoryDumper {
public:
    static void dumpToFile(int id, MemoryBlock* block, const std::string& dumpPath);
};

#endif // MEMORYDUMPER_H