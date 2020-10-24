#pragma once

#include <unistd.h>
#include <vector>

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

    /* Read and return a single byte at "address" */
    char ReadByte(void *address);

    /* Read and return an int at "address" */
    int ReadInt(void *address);

    /* Prints mapped memory regions on the heap and stack */
    void PrintRegionInfo(std::ostream &os);

    /* Returns that the process exists */
    bool IsValid();

    /* Returns that the wrapped process is still running */
    bool IsRunning();

    void* Find(const char* data, const char* pattern);

    bool Read(void* address, void* buffer, size_t size);
    bool Write(void* address, void* buffer, size_t size);

    unsigned long GetCallAddress(void* address);

    MemoryRegion* GetRegionOfAddress(void* address);

  private:
    const pid_t process_id_;

    std::vector<MemoryRegion> GetMappedMemory();
};
} // namespace ProcessMemoryViewer