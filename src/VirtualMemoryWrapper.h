#pragma once

#include <unistd.h>
#include <vector>

namespace ProcessMemoryViewer {
/* 
Wrapper class that provides convenient methods to read/write to virtual memory of another process
*/
class VirtualMemoryWrapper {
  public:
    VirtualMemoryWrapper(pid_t process_id);

    /* Read and return a single byte at "address" */
    char ReadByte(void *address);

    /* Read and return an int at "address" */
    int ReadInt(void *address);

  private:
    const pid_t process_id_;
};
} // namespace ProcessMemoryViewer