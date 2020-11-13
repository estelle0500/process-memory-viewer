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

    /* Search addresses for "value" */
    template<typename T>
    std::vector<void *> SearchValue(T value, double eps) {
        std::vector<void*> matched_addresses;

        std::cout << "Address" << "\t\t\t\t" << "Value" << std::endl;
        for (const auto &page_data : memory_) {
            char *base_addr = (char*) page_data.first;

            for (size_t offset = 0; offset < PAGE_SIZE; offset += sizeof(value)) {
                T val = *(T*) &page_data.second[offset];

                if ((std::is_floating_point<T>::value && fabs(val - value) < eps)
                    || val == value) {
                    void *addr = base_addr + offset;
                    std::cout << addr << "\t\t\t" << val << std::endl;
                    matched_addresses.push_back(addr);
                } 
            }
        }
        return matched_addresses;
    }

    template<typename T>
    std::vector<void *> SearchValue(T value) {
        return SearchValue(value, 0.0);
    }

  private:
    std::unordered_map<void *, std::vector<unsigned char> > memory_;
    size_t PAGE_SIZE;
};

/* Container for many snapshots */
class MemorySnapshotManager {
  public:
    /* Save a snapshot of current process memory 
       Returns an id that can be used to refer to the snapshot */
    unsigned int SaveSnapshot(const VirtualMemoryWrapper &memory_wrapper);

    /* Remove a snapshot given the current id */
    void DeleteSnapshot(unsigned int id);

    /* Returns the amount of snapshots saved */
    int GetSize();

    /* Compare old and new snapshots */
    void PrintComparison(unsigned int old_snapshot_id, unsigned int new_snapshot_id);

  private:
    std::vector<MemorySnapshot> snapshots_;
};
} // namespace ProcessMemoryViewer