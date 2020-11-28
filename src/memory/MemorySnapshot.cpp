#include "MemorySnapshot.h"

namespace ProcessMemoryViewer {
MemorySnapshot::MemorySnapshot(const VirtualMemoryWrapper &memory_wrapper){
    PAGE_SIZE = sysconf(_SC_PAGE_SIZE);
    auto regions = memory_wrapper.memory_regions();

    for (const auto &region : regions) {
        size_t size = (size_t)((char*) region.end_ - (char*) region.begin_);

        for (size_t i = 0; i < size / PAGE_SIZE; ++i) {
            void *page_start = (char*) region.begin_ + i * PAGE_SIZE;
            memory_.emplace(page_start, memory_wrapper.Read(page_start, PAGE_SIZE));
        }
    }
}

void PrintDifferencesInRegion(void*, const std::vector<unsigned char>&, const std::vector<unsigned char>&);

void MemorySnapshot::PrintAddressDifferences(const MemorySnapshot &other_snapshot) const {
    for (const auto &record : memory_) {
        void *addr_start = record.first;
        void *addr_end = (char*) addr_start + record.second.size();
        if (other_snapshot.memory_.find(addr_start) == other_snapshot.memory_.end()) {
            // Not found in the other snapshot, so it's been deleted
            std::cout << "Deleted: new region of addresses from " << addr_start
                      << " to " << addr_end << std::endl;
            continue;
        }

        const std::vector<unsigned char> other_record_buf = other_snapshot.memory_.at(addr_start);
        PrintDifferencesInRegion(addr_start, record.second, other_record_buf);
    }

    for (const auto &other_record : other_snapshot.memory_) {
        void *addr_start = other_record.first;
        void *addr_end = (char*) addr_start + other_record.second.size();
        if (memory_.find(addr_start) == memory_.end()) {
            // Not found in this snapshot, so it's been added
            std::cout << "Deleted: new region of addresses from " << addr_start
                      << " to " << addr_end << std::endl;
        }
    }
}

void PrintDifferencesInRegion(void *addr_start, const std::vector<unsigned char> &buffer, 
                              const std::vector<unsigned char> &other_buffer) {
    using std::cout;
    constexpr size_t BUFFER_SIZE = 256;
    char print_buffer[BUFFER_SIZE];

    for (size_t i = 0; i < buffer.size(); ++i) {
        if (buffer[i] != other_buffer[i]) {
            unsigned old_value = static_cast<unsigned>(buffer[i]);
            unsigned new_value = static_cast<unsigned>(other_buffer[i]);
            snprintf(print_buffer, BUFFER_SIZE,
                    "Modified at address %p, Old value: 0x%.2x, New value: 0x%.2x\n" ,
                     (char*) addr_start + i, old_value, new_value);
            cout << print_buffer;
        }
    }
}

unsigned int MemorySnapshotManager::SaveSnapshot(VirtualMemoryWrapper &memory_wrapper) {
    unsigned int id = snapshots_.size();
    snapshots_.emplace_back(MemorySnapshot(memory_wrapper));
    return id;
}

void MemorySnapshotManager::PrintComparison(unsigned int old_snapshot_id, unsigned int new_snapshot_id) {
    if (old_snapshot_id >= snapshots_.size()) {
        std::cout << "Invalid id for old snapshot" << std::endl;
        return;
    } else if (new_snapshot_id >= snapshots_.size()) {
        std::cout << "Invalid id for new snapshot" << std::endl;
        return;
    }

    const MemorySnapshot &old_snapshot = snapshots_.at(old_snapshot_id);
    const MemorySnapshot &new_snapshot = snapshots_.at(new_snapshot_id);
    old_snapshot.PrintAddressDifferences(new_snapshot);
}

void MemorySnapshotManager::DeleteSnapshot(unsigned int id) {
    if (id >= snapshots_.size()) {
        std::cout << "Invalid id for snapshot" << std::endl;
        return;
    }

    snapshots_.erase(snapshots_.begin() + id);
}

int MemorySnapshotManager::GetSize() {
    return snapshots_.size();
}
} // namespace ProcessMemoryViewer