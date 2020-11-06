#pragma once

#include <vector>
#include <unordered_map>

#include "MemoryRegion.h"
#include "VirtualMemoryWrapper.h"

namespace ProcessMemoryViewer {
/* A snapshot contains the memory state at one point in time */
class MemorySnapshot {
  public:
    /* Save a snapshot of current process memory */
    MemorySnapshot(const VirtualMemoryWrapper &memory_wrapper);

    /* Prints all addresses that have a different value between this and other snapshot 
       Differentiates between added, deleted, and modified addresses */
    void PrintAddressDifferences(const MemorySnapshot &other_snapshot) const;

  private:
    std::unordered_map<void *, std::vector<char> > memory_;
};
} // namespace ProcessMemoryViewer