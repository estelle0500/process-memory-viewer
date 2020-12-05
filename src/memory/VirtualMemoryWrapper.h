#pragma once

#include <unistd.h>
#include <sys/uio.h>
#include <vector>
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

    /* Read process memory wrapper for different buffers */
    std::vector<unsigned char> Read(void *address, size_t size) const;

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

    void WriteString(void *address, std::string value) {
      struct iovec remote_iov{address, value.length() + 1};
      struct iovec local_iov{(void*) value.c_str(), value.length() + 1};
      process_vm_writev(process_id_, &local_iov, 1, &remote_iov, 1, 0);
    }

    /* Prints mapped memory regions */
    void PrintRegionInfo() const;

    /* Replace active region maps */
    void ParseMaps();

    /* Returns region of a given memory address */
    const MemoryRegion& GetRegionOfAddress(void *address);

    /* Print all variables in a region */
    void PrintRegion(int index, size_t buffer_size);

    const std::vector<MemoryRegion>& memory_regions() const {
        return memory_regions_;
    }

    void SetPid(pid_t pid);


  private:
    pid_t process_id_;
    std::vector<MemoryRegion> memory_regions_;
};
} // namespace ProcessMemoryViewer