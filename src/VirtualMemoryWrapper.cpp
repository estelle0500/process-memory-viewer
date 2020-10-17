#include "VirtualMemoryWrapper.h"
#include <cassert>
#include <string>
#include <sstream>
#include <fstream>
#include <sys/uio.h>

namespace ProcessMemoryViewer {
VirtualMemoryWrapper::VirtualMemoryWrapper(pid_t process_id) : process_id_(process_id) {}

char VirtualMemoryWrapper::ReadByte(void *address) {
    char byte_read;
    struct iovec remote_iov{address, 1};
    struct iovec local_iov{&byte_read, 1};
    ssize_t num_bytes_read = process_vm_readv(process_id_, &local_iov, 1, &remote_iov, 1, 0);

    assert(num_bytes_read == 1);
    return byte_read;
}

int VirtualMemoryWrapper::ReadInt(void *address) {
    int int_read;
    struct iovec remote_iov{address, sizeof(int)};
    struct iovec local_iov{&int_read, sizeof(int)};
    ssize_t num_bytes_read = process_vm_readv(process_id_, &local_iov, 1, &remote_iov, 1, 0);

    assert(num_bytes_read == sizeof(int));
    return int_read;
}

void VirtualMemoryWrapper::PrintMappedMemory(std::ostream &os) {
    std::vector<MemoryRegion> regions = this->GetMappedMemory();
    for (const MemoryRegion &region : regions) {
        if (region.pathname() == "[stack]" || region.pathname() == "[heap]") {
            os << region;
        } 
    }
}

std::vector<MemoryRegion> VirtualMemoryWrapper::GetMappedMemory() {
    constexpr char PROC_DIRECTORY[] = "/proc";
    constexpr char PATH_SEP[] = "/";
    constexpr char MAPS_FILE[] = "maps";
    
    std::ostringstream oss;
    oss << PROC_DIRECTORY << PATH_SEP << process_id_ << PATH_SEP << MAPS_FILE;
    const std::string maps_filepath = oss.str();
    std::ifstream maps_stream(maps_filepath);

    std::vector<MemoryRegion> regions;

    while (true) {
        std::string line;
        getline(maps_stream, line);
        if (maps_stream.eof()) {
            break;
        }
        std::istringstream single_line_stream(line);

        MemoryRegion region;
        single_line_stream >> region;
        regions.emplace_back(region);
    }
    return regions;
}
} // namespace ProcessMemoryViewer