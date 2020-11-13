#pragma once

#include <iostream>
#include <sstream>
#include <vector>

#include "memory/VirtualMemoryWrapper.h"
#include "memory/MemorySnapshot.h"
#include "Watchlist.h"

namespace ProcessMemoryViewer {
class CommandLineInterface {
  public:
    CommandLineInterface(VirtualMemoryWrapper &memory_wrapper, std::ostream &out_stream, Watchlist &watchlist)
        : memory_wrapper_(memory_wrapper), out_stream_(out_stream), watchlist_(watchlist) {}

    void HandleInput(std::string input);

  private:
    VirtualMemoryWrapper &memory_wrapper_;
    std::ostream &out_stream_;
    MemorySnapshotManager snapshot_manager_;
    Watchlist &watchlist_;
    double eps_ = 0;
};

pid_t get_pid_from_name(std::string procName);
} // namespace ProcessMemoryViewer