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

MemoryRegion* VirtualMemoryWrapper::GetRegionOfAddress(void* address){
    for (size_t i = 0; i < Memory_Regions.size(); i++) {
        unsigned long begin         = (unsigned long) Memory_Regions[i].begin_;
        unsigned long end           = (unsigned long) Memory_Regions[i].end_;
        unsigned long ul_address    = (unsigned long) address;

        if (begin < ul_address && end >= ul_address) {
            return &Memory_Regions[i];
        }
    }
}

void VirtualMemoryWrapper::PrintRegionInfo(std::ostream &os) {
    std::vector<MemoryRegion> regions = this->GetMappedMemory();
    for (const MemoryRegion &region : regions) {
        if (region.pathname() == "[stack]" || region.pathname() == "[heap]") {
            os << region;
        }
    }
}

void VirtualMemoryWrapper::ParseMaps(){
    Memory_Regions.clear();
    Memory_Regions = GetMappedMemory();
    return;
}

std::vector<MemoryRegion> VirtualMemoryWrapper::GetMappedMemory() {
    int count = 0;
    std::ostringstream oss;
    oss << PROC_DIRECTORY << PATH_SEP << process_id_ << PATH_SEP << MAPS_FILE;      // proc/pid/maps
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
        region.id = count;
        count++;

        // Permissions
        region.readable     = (region.permissions_[0] == 'r');
        region.writable     = (region.permissions_[1] == 'w');
        region.executable   = (region.permissions_[2] == 'x');
        region.shared       = (region.permissions_[3] != '-');

        single_line_stream >> region;
        regions.emplace_back(region);
    }
    return regions;
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
    struct iovec local[1];
    struct iovec remote[1];

    local[0].iov_base = buffer;
    local[0].iov_len = size;
    remote[0].iov_base = address;
    remote[0].iov_len = size;

    return (process_vm_readv(process_id_, local, 1, remote, 1, 0) == size);
}

void *VirtualMemoryWrapper::FindPattern(const char *data, const char *pattern) {
    char buffer[4];

    if(Memory_Regions.empty()){
        Memory_Regions = GetMappedMemory();
    }
    unsigned long begin = (unsigned long)Memory_Regions[0].begin_;
    unsigned long end = (unsigned long)Memory_Regions[0].end_;

    size_t len = strlen(pattern);
    size_t chunksize = sizeof(buffer);
    size_t total = end - begin;
    size_t chunk = 0;

    while(total) {
        size_t readsize = (total < chunksize) ? total : chunksize;
        size_t readaddr = begin + (chunksize * chunk);

        bzero(buffer, chunksize);

        if(Read((void*) readaddr, buffer, readsize)) {
            for(size_t b = 0; b < readsize; b++) {
                size_t matches = 0;

                while(buffer[b + matches] == data[matches] || pattern[matches] != 'x') {
                    matches++;

                    if(matches == len) {
                        return (char*) (readaddr + b);
                    }
                }
            }
        }
        total -= readsize;
        chunk++;
    }
    return NULL;
}



void VirtualMemoryWrapper::PrintRegion(int index, size_t buffer_size){
    unsigned long begin = (unsigned long)Memory_Regions[index].begin_;
    unsigned long end = (unsigned long)Memory_Regions[index].end_;
    char buffer[buffer_size];
    size_t chunksize = sizeof(buffer);
    size_t total = end - begin;
    size_t chunk = 0;

    std::cout << "size: " << Memory_Regions.size() << std::endl;

    while(total) {
        size_t readsize = (total < chunksize) ? total : chunksize;
        size_t readaddr = begin + (chunksize * chunk);

        bzero(buffer, chunksize);

        if(Read((void*) readaddr, buffer, readsize)) {
            std::cout << reinterpret_cast<void *>(readaddr) << ":\t\t\t" << Read<int>(reinterpret_cast<void *>(readaddr)) << "\t\t\t" << buffer << std::endl;
        }

        total -= readsize;
        chunk++;
    }
}

void VirtualMemoryWrapper::PrintRegionBounds() {
    for(size_t i = 0; i < Memory_Regions.size(); i++) {
        std::cout << "Checking region " << i << ": " << Memory_Regions[i].begin_ << "\t" << Memory_Regions[i].end_ << std::endl;
    }
}

//template<typename T>
//std::vector<void *> VirtualMemoryWrapper::FindValues(T value) {
//    std::vector<void*> matchedAddresses;
//    for (int i = 0; i < Memory_Regions.size()-1; ++i) {
//        unsigned long begin     = (unsigned long)Memory_Regions[i].begin_;
//        unsigned long end       = (unsigned long)Memory_Regions[i].end_;
//
//        for (unsigned long i = begin; i < end; i += 4) {
//            if(int val = Read<int>((void*)i)) {
//                if(val == value){
//                    //std::cout << "Matched" << value << "at: \t " << (void*)i << std::endl;
//                    std::cout << (void*)i << "\t\t\t" << value;
//                    matchedAddresses.push_back((void*)i);
//                }
//            }
//        }
//    }
//    return matchedAddresses;
//}

    template<typename T>
    void VirtualMemoryWrapper::PrintMemoryValue(void *address, T value) {
        
    }
} // namespace ProcessMemoryViewer