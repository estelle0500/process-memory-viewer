#include "MemorySnapshot.h"

namespace ProcessMemoryViewer {
MemorySnapshot::MemorySnapshot(const VirtualMemoryWrapper &memory_wrapper) {
    auto regions = memory_wrapper.memory_regions();
    size_t page_size = sysconf(_SC_PAGE_SIZE);

    for (const auto &region : regions) {
        size_t size = (size_t)((char*) region.end_ - (char*) region.begin_);

        for (size_t i = 0; i < size / page_size; ++i) {
            void *page_start = (char*) region.begin_ + i * page_size;
            memory_.emplace(page_start, memory_wrapper.Read(page_start, page_size));
        }
    }
}

void PrintDifferencesInRegion(void*, const std::vector<char>&, const std::vector<char>&);

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

        const std::vector<char> other_record_buf = other_snapshot.memory_.at(addr_start);
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

void PrintDifferencesInRegion(void *addr_start, const std::vector<char> &buffer, 
                              const std::vector<char> &other_buffer) {
    using std::cout;
    constexpr size_t BUFFER_SIZE = 256;
    char print_buffer[BUFFER_SIZE];

    for (size_t i = 0; i < buffer.size(); ++i) {
        if (buffer[i] != other_buffer[i]) {
            snprintf(print_buffer, BUFFER_SIZE,
                    "Modified at address %p, Old value: 0x%.2x, New value: 0x%.2x\n" ,
                     (char*) addr_start + i, buffer[i], other_buffer[i]);
            cout << print_buffer;
        }
    }
}
} // namespace ProcessMemoryViewer