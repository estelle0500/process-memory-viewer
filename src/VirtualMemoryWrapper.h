#pragma once

#include <unistd.h>
#include <vector>
#include <sys/uio.h>
#include <cmath>
#include <type_traits>

#include "MemoryRegion.h"

namespace ProcessMemoryViewer {
/* 
Wrapper class that provides convenient methods to read/write to virtual memory of another process
*/
class VirtualMemoryWrapper {
    static constexpr char PROC_DIRECTORY[] = "/proc";
    static constexpr char PATH_SEP[] = "/";
    static constexpr char MAPS_FILE[] = "maps";

  public:
    VirtualMemoryWrapper(pid_t process_id);
      
    /* Read and return data at "address" */
    template <class T>
    T Read(void *address) {
      T buffer;
      struct iovec remote_iov{address, sizeof(T)};
      struct iovec local_iov{&buffer, sizeof(T)};
      ssize_t num_bytes_read = process_vm_readv(process_id_, &local_iov, 1, &remote_iov, 1, 0);

      if (num_bytes_read != sizeof(T)) {
          // perror("VirtualMemoryWrapper : Read failed");
      }
      return buffer;
    }

    /* Write data at "address" */
    template <class T>
    void Write(void *address, T data) {
      struct iovec remote_iov{address, sizeof(T)};
      struct iovec local_iov{&data, sizeof(T)};
      ssize_t num_bytes_written = process_vm_writev(process_id_, &local_iov, 1, &remote_iov, 1, 0);

      if (num_bytes_written != sizeof(T)) {
          perror("VirtualMemoryWrapper : Write failed");
      }
    }

    /* Search addresses for "value" */
    template<typename T>
    std::vector<void *> SearchValue(T value) {
        std::vector<void*> matched_addresses;

        std::cout << "Address" << "\t\t\t\t" << "value" << std::endl;
        for (size_t i = 0; i < memory_regions_.size(); ++i) {
            unsigned long begin     = (unsigned long) memory_regions_[i].begin_;
            unsigned long end       = (unsigned long) memory_regions_[i].end_;

            for (unsigned long addr = begin; addr < end; addr += sizeof(value)) {
                T val = Read<T>((void*)addr);
                if ((std::is_floating_point<T>::value && fabs(val - value) < ep)
                    || val == value) {
                    std::cout << (void*)addr << "\t\t\t" << val << std::endl;
                    matched_addresses.push_back((void*)addr);
                } 
            }
        }
        return matched_addresses;
    }

    /* Prints mapped memory regions */
    void PrintRegionInfo() const;

    /* Returns that the process exists */
    bool IsValid() const;

    /* Returns that the wrapped process is still running */
    bool IsRunning() const;

    /* Read process memory wrapper for different buffers */
    std::vector<unsigned char> Read(void *address, size_t size) const;

    /* Replace active region maps */
    void ParseMaps();

    /* Returns region of a given memory address */
    const MemoryRegion& GetRegionOfAddress(void *address);

    /* Print all variables in a region */
    void PrintRegion(int index, size_t buffer_size);

    pid_t process_id() const {
        return process_id_;
    }

    const std::vector<MemoryRegion>& memory_regions() const {
        return memory_regions_;
    }

    void set_ep(double new_ep) {
        ep = new_ep;
    }

  private:
    const pid_t process_id_;
    double ep = 0.01;
    std::vector<MemoryRegion> memory_regions_;
};
} // namespace ProcessMemoryViewer