#pragma once

#include <unistd.h>
#include <vector>
#include <sys/uio.h>
#include <cmath>

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
    std::vector<MemoryRegion> Memory_Regions;   // Regions

    double ep = 0.01;

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
        std::vector<void*> matchedAddresses;

        std::cout << "Address" << "\t\t\t\t" << "value" << std::endl;
        for (int i = 0; i < Memory_Regions.size()-1; ++i) {
            unsigned long begin     = (unsigned long)Memory_Regions[i].begin_;
            unsigned long end       = (unsigned long)Memory_Regions[i].end_;

            for (unsigned long i = begin; i < end; i += sizeof(value)) {
                if(T val = Read<T>((void*)i)) {
                    if(fabs(val - value) < ep){
                        //std::cout << "Matched" << value << "at: \t " << (void*)i << std::endl;
                        std::cout << (void*)i << "\t\t\t" << val << std::endl;
                        matchedAddresses.push_back((void*)i);
                    }
                }
            }
        }
        return matchedAddresses;
    }

    /* Prints mapped memory regions on the heap and stack */
    void PrintRegionInfo(std::ostream &os);

    /* Returns that the process exists */
    bool IsValid();

    /* Returns that the wrapped process is still running */
    bool IsRunning();

    /* Read process memory wrapper for different buffers
     * TODO: remove this after presentation. */
    bool Read(void *address, void *buffer, size_t size);

    /* Replace active region maps */
    void ParseMaps();

    /* Returns region of a given memory address */
    MemoryRegion *GetRegionOfAddress(void *address);

    /* Print all variables in a region */
    void PrintRegion(int index, size_t buffer_size);

    /* Print all region start & end addresses */
    void PrintRegionBounds();

    pid_t process_id() const {
        return process_id_;
    }

  private:
    const pid_t process_id_;

    std::vector<MemoryRegion> GetMappedMemory();
};
} // namespace ProcessMemoryViewer