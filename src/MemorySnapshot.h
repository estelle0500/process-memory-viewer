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
    std::unordered_map<void *, std::vector<unsigned char> > memory_;
};

/* Container for many snapshots */
class MemorySnapshotManager {
  public:
    /* Save a snapshot of current process memory 
       Returns an id that can be used to refer to the snapshot */
    unsigned int SaveSnapshot(const VirtualMemoryWrapper &memory_wrapper);

    /* Compare old and new snapshots */
    void PrintComparison(unsigned int old_snapshot_id, unsigned int new_snapshot_id);

  private:
    std::vector<MemorySnapshot> snapshots_;
};
} // namespace ProcessMemoryViewer