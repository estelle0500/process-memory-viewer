#include "VirtualMemoryWrapper.h"
#include <cassert>
#include <string>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <sys/uio.h>
#include <inttypes.h>
#include <dirent.h>
#include <string.h>>
#include <iostream>

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

    MemoryRegion* VirtualMemoryWrapper::GetRegionOfAddress(void* address){
        for(size_t i = 0; i < Memory_Regions.size(); i++) {
            unsigned long begin = (unsigned long)Memory_Regions[i].begin_;
            unsigned long end = (unsigned long)Memory_Regions[i].end_;
            if(begin > (unsigned long) address
            && (begin + end) <= (unsigned long) address) {
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

    std::vector<MemoryRegion> VirtualMemoryWrapper::GetMappedMemory() {
        std::ostringstream oss;
        oss << PROC_DIRECTORY << PATH_SEP << process_id_ << PATH_SEP << MAPS_FILE;      // proc/pid/maps
        const std::string maps_filepath = oss.str();                                    // gets the string
        std::ifstream maps_stream(maps_filepath);                                       // file stream

        std::vector<MemoryRegion> regions;                                              // region vector

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

    bool VirtualMemoryWrapper::IsValid() {
        return process_id_ != -1;
    }

    bool VirtualMemoryWrapper::IsRunning() {
        if(!IsValid()){
            return false;
        }
        struct stat sts;
        if(stat((("/proc/" + process_id_)), &sts) != -1) {
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

    void *VirtualMemoryWrapper::Find(const char *data, const char *pattern) {
        char buffer[4];

        if(Memory_Regions.empty()){
            Memory_Regions = GetMappedMemory();
        }
        unsigned long begin = (unsigned long)Memory_Regions[0].begin_;
        unsigned long end = (unsigned long)Memory_Regions[0].end_;

        size_t len = strlen(pattern);
        size_t chunksize = sizeof(buffer);
        size_t total = end - begin;
        size_t chunkno = 0;

        while(total) {
            size_t readsize = (total < chunksize) ? total : chunksize;
            size_t readaddr = begin + (chunksize * chunkno);

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
            chunkno++;
        }

        return NULL;
    }

    void VirtualMemoryWrapper::PrintRegion(int index, size_t buffer_size){
        if(Memory_Regions.empty()){
            Memory_Regions = GetMappedMemory();
        }
        unsigned long begin = (unsigned long)Memory_Regions[index].begin_;
        unsigned long end = (unsigned long)Memory_Regions[index].end_;
        char buffer[buffer_size];
        size_t chunksize = sizeof(buffer);
        size_t total = end - begin;
        size_t chunk = 0;

        while(total) {
            size_t readsize = (total < chunksize) ? total : chunksize;
            size_t readaddr = begin + (chunksize * chunk);

            bzero(buffer, chunksize);

            std::cout << readaddr << ":\t\t" << ReadInt(reinterpret_cast<void *>(readaddr)) << std::endl;

            total -= readsize;
            chunk++;
        }
    }

} // namespace ProcessMemoryViewer