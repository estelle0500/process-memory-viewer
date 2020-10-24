#pragma once

#include <unistd.h>
#include <vector>
#include <sys/uio.h>

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
    uintptr_t va() const;                       // Current virtual address
    uintptr_t last_va() const;                  // Last virtual address
    std::vector<MemoryRegion> Memory_Regions;   // Regions

    VirtualMemoryWrapper(pid_t process_id);
      
    /* Read and return data at "address" */
    template <class T>
    T Read(void *address) {
      T buffer;
      struct iovec remote_iov{address, sizeof(T)};
      struct iovec local_iov{&buffer, sizeof(T)};
      ssize_t num_bytes_read = process_vm_readv(process_id_, &local_iov, 1, &remote_iov, 1, 0);

      if (num_bytes_read != sizeof(T)) {
          perror("VirtualMemoryWrapper: Read failed: ");
      }
      return buffer;
    }

    /* Prints mapped memory regions on the heap and stack */
    void PrintRegionInfo(std::ostream &os);

    /* Returns that the process exists */
    bool IsValid();

    /* Returns that the wrapped process is still running */
    bool IsRunning();

    /* Find a pattern in memory
      *
      * usage:
      *  find("\xFA\x00\x00\x00\x00\x22\x33\x44\x55\xDD\x34",
        "x????xxxxxx")
      * */
    void *FindPattern(const char *data, const char *pattern);

    std::vector<void*> FindValues(int value);

    /* Read process memory wrapper for different buffers */
    bool Read(void *address, void *buffer, size_t size);

    /* Replace active region maps */
    void ParseMaps();

    /* Returns region of a given memory address */
    MemoryRegion *GetRegionOfAddress(void *address);

    /* Print all variables in a region */
    void PrintRegion(int index, size_t buffer_size);

    /* Print all region start & end addresses */
    void PrintRegionBounds();

  private:
    const pid_t process_id_;

    std::vector<MemoryRegion> GetMappedMemory();
};
} // namespace ProcessMemoryViewer