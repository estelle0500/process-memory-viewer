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
    using std::string;
    using std::vector;
    static double eps_ = 0.01;

    class CommandLineInterface {
    public:
        CommandLineInterface(VirtualMemoryWrapper &memory_wrapper, Watchlist &watchlist, ProcessTracer &tracer,
                             History &history)
                : memory_wrapper_(memory_wrapper), watchlist_(watchlist), tracer_(tracer), history_(history) {}

        void HandleInput(std::string input);

    private:
        /* Helper function to split */
        vector<string> split(string str);

        MemoryList last_results;

        VirtualMemoryWrapper &memory_wrapper_;
        MemorySnapshotManager snapshot_manager_;
        Watchlist &watchlist_;
        ProcessTracer &tracer_;
        History &history_;
    };

    string get_proc_name(pid_t pid);
    pid_t get_proc_pid(std::string procName);
} // namespace ProcessMemoryViewer