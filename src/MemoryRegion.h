#pragma once

#include <iostream>

namespace ProcessMemoryViewer {
    class MemoryRegion {
    public:
        int id;
        friend std::ostream &operator<<(std::ostream &os, const MemoryRegion &region);

        friend std::istream &operator>>(std::istream &is, MemoryRegion &region);

        std::string pathname() const {
            return this->pathname_;
        }

        std::string permissions_;

        // Permissions
        bool readable;
        bool writable;
        bool executable;
        bool shared;

        long offset_;
        std::string dev_;
        long inode_;
        std::string pathname_;
        std::string filename_;

        void *begin_;
        void *end_;


    private:
    };
} // namespace ProcessMemoryViewer