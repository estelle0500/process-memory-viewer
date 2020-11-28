#pragma once

#include <vector>
#include <unordered_map>
#include <cstring>

#include "MemoryRegion.h"
#include "VirtualMemoryWrapper.h"
#include "MemoryHistory.h"

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
    MemoryList SearchValue(T value, double eps, value_type type) {
        MemoryList res;

        for (const auto &page_data : memory_) {
            char *base_addr = (char*) page_data.first;

            for (size_t offset = 0; offset < PAGE_SIZE; offset += sizeof(value)) {
                T val = *(T*) &page_data.second[offset];

                if ((std::is_floating_point<T>::value && fabs(val - value) < eps)
                    || val == value) {
                    void *addr = base_addr + offset;
                    res.addresses.push_back(addr);
                    search_value v;
                    v.type = type;
                    switch(v.type){
                        case VALUE_S32: {
                            v.value.s32 = val;
                        } break;
                        case VALUE_F32: {
                            v.value.f32 = val;
                        } break;
                        case VALUE_F64: {
                            v.value.f64 = val;
                        } break;
                    }
                    res.values.push_back(v);
                }
            }
        }

        return res;
    }

    /* Search for addresses that match the string */
    std::vector<void*> SearchString(std::string pattern);

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