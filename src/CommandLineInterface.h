#pragma once

#include <iostream>
#include <sstream>
#include <vector>

#include "memory/VirtualMemoryWrapper.h"
#include "memory/MemorySnapshot.h"
#include "Watchlist.h"
#include "ProcessTracer.h"
#include "memory/MemoryHistory.h"

namespace ProcessMemoryViewer {
    static double eps_ = 0.01;

    class CommandLineInterface {
  public:
    CommandLineInterface(VirtualMemoryWrapper &memory_wrapper, Watchlist &watchlist, ProcessTracer &tracer,
                         History &history)
        : memory_wrapper_(memory_wrapper), watchlist_(watchlist), tracer_(tracer), history_(history) {}

    void HandleInput(std::string input);

  private:
    VirtualMemoryWrapper &memory_wrapper_;
    MemorySnapshotManager snapshot_manager_;
    Watchlist &watchlist_;
    ProcessTracer &tracer_;
    History &history_;
};

pid_t get_pid_from_name(std::string procName);
} // namespace ProcessMemoryViewer