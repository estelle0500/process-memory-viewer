#include "MemoryRegion.h"

namespace ProcessMemoryViewer {

std::ostream& operator<<(std::ostream &os, const MemoryRegion &region) {
    os << "-----[Memory Region]-----" << "\n"
       << "Begins at:\t" << region.begin_ << "\n"
       << "Ends at:\t" << region.end_ << "\n"
       << "Belongs to:\t" << region.pathname_ << "\n";
    return os;
}

std::istream& operator>>(std::istream &is, MemoryRegion &region) {
    is >> region.begin_;
    is.ignore(1, '-');
    is >> region.end_ >> region.permissions_ >> region.offset_ >> region.dev_
       >> region.inode_ >> region.pathname_;
    return is;
}


} // namespace ProcessMemoryViewer