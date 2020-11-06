#include "VirtualMemoryWrapper.h"
#include <string>
#include <sstream>
#include <fstream>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>

namespace ProcessMemoryViewer {
VirtualMemoryWrapper::VirtualMemoryWrapper(pid_t process_id) : process_id_(process_id) {
    ParseMaps();
}

const MemoryRegion& VirtualMemoryWrapper::GetRegionOfAddress(void* address){
    for (size_t i = 0; i < memory_regions_.size(); i++) {
        unsigned long begin         = (unsigned long) memory_regions_[i].begin_;
        unsigned long end           = (unsigned long) memory_regions_[i].end_;
        unsigned long ul_address    = (unsigned long) address;

        if (begin <= ul_address && end > ul_address) {
            return memory_regions_[i];
        }
    }
}

void VirtualMemoryWrapper::PrintRegionInfo() {
    for (size_t i = 0; i < memory_regions_.size(); i++) {
        std::cout << "Checking region " << i << ": " << std::endl;
        std::cout << memory_regions_[i] << std::endl;
    }
}

void VirtualMemoryWrapper::ParseMaps(){
    memory_regions_.clear();
    
    std::ostringstream oss;
    oss << PROC_DIRECTORY << PATH_SEP << process_id_ << PATH_SEP << MAPS_FILE;      // proc/pid/maps
    const std::string maps_filepath = oss.str();
    std::ifstream maps_stream(maps_filepath);

    for (size_t count = 0; ; ++count) {
        std::string line;
        getline(maps_stream, line);
        if (maps_stream.eof()) {
            break;
        }
        std::istringstream single_line_stream(line);

        MemoryRegion region;
        region.id = count;

        // Permissions
        region.readable     = (region.permissions_[0] == 'r');
        region.writable     = (region.permissions_[1] == 'w');
        region.executable   = (region.permissions_[2] == 'x');
        region.shared       = (region.permissions_[3] != '-');

        single_line_stream >> region;
        memory_regions_.emplace_back(region);
    }
}

bool VirtualMemoryWrapper::IsValid() {
    return process_id_ != -1;
}

bool VirtualMemoryWrapper::IsRunning() {
    if (!IsValid()){
        return false;
    }
    struct stat sts;
    if (stat((("/proc/" + process_id_)), &sts) != -1) {
        return false;
    }
}

bool VirtualMemoryWrapper::Read(void* address, void* buffer, size_t size) {
    struct iovec local{buffer, size};
    struct iovec remote{address, size};
    return (process_vm_readv(process_id_, &local, 1, &remote, 1, 0) == size);
}

void VirtualMemoryWrapper::PrintRegion(int index, size_t buffer_size){
    unsigned long begin = (unsigned long)memory_regions_[index].begin_;
    unsigned long end = (unsigned long)memory_regions_[index].end_;
    char buffer[buffer_size];
    size_t chunksize = sizeof(buffer);
    size_t total = end - begin;
    size_t chunk = 0;

    std::cout << "size: " << memory_regions_.size() << std::endl;

    while (total) {
        size_t readsize = (total < chunksize) ? total : chunksize;
        size_t readaddr = begin + (chunksize * chunk);

        bzero(buffer, chunksize);

//        int val;
//        std::cout << reinterpret_cast<void *>(readaddr) << ":\t\t\t"
//        << (val = Read<int>(reinterpret_cast<void *>(readaddr))) << "\t\t\t"
//        <<  std::to_string(val) << "\n";
        if (Read((void*) readaddr, buffer, readsize)) {
            std::cout << reinterpret_cast<void *>(readaddr) << ":\t\t\t" << Read<int>(reinterpret_cast<void *>(readaddr)) << "\t\t\t" << buffer << std::endl;
        }

        total -= readsize;
        chunk++;
    }
}
} // namespace ProcessMemoryViewer