#pragma once

#include <iostream>

namespace ProcessMemoryViewer {
class MemoryRegion {
  public:
    friend std::ostream& operator<<(std::ostream &os, const MemoryRegion &region);
    friend std::istream& operator>>(std::istream &is, MemoryRegion &region);

    std::string pathname() const {
        return this->pathname_;
    }

  private:
    void *begin_;
    void *end_;
    std::string permissions_;
    long offset_;
    std::string dev_;
    long inode_;
    std::string pathname_;
};
} // namespace ProcessMemoryViewer