#pragma once

#include <iostream>
#include <sstream>
#include <vector>

#include "memory/VirtualMemoryWrapper.h"
#include "memory/MemorySnapshot.h"
#include "Watchlist.h"
#include "ProcessTracer.h"

namespace ProcessMemoryViewer {
class CommandLineInterface {
  public:
    CommandLineInterface(VirtualMemoryWrapper &memory_wrapper, Watchlist &watchlist, ProcessTracer &tracer)
        : memory_wrapper_(memory_wrapper), watchlist_(watchlist), tracer_(tracer) {}

    void HandleInput(std::string input);

  private:
    VirtualMemoryWrapper &memory_wrapper_;
    MemorySnapshotManager snapshot_manager_;
    Watchlist &watchlist_;
    ProcessTracer &tracer_;

    double eps_ = 0.01;
};

pid_t get_pid_from_name(std::string procName);
} // namespace ProcessMemoryViewer