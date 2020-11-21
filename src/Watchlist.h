#include <vector>
#include "memory/VirtualMemoryWrapper.h"


#ifndef PROCESS_MEMORY_VIEWER_WATCHLIST_H
#define PROCESS_MEMORY_VIEWER_WATCHLIST_H

/* Saves a list of addresses to monitor addresses of a given type */
namespace ProcessMemoryViewer {

    /* A list of saved addresses */
    class Watchlist {
    public:
        Watchlist(VirtualMemoryWrapper &memoryWrapper)
                : memory_wrapper_(memoryWrapper) {}
        /* Add a new address to watch */
        void Add(void *addr);

        /* Print all addresses and current values in the watchlist */
        void Print();

        /* Print only addresses with different values from the original */
        void PrintChanged();

        /* Returns the amount of watched addresses */
        int GetSize();
    private:
        VirtualMemoryWrapper &memory_wrapper_;
        std::vector<void*> addresses_;
        std::vector<int> original_values_;

        /* Format a single address*/
        void PrintAddress(int id);
    };
}

#endif //PROCESS_MEMORY_VIEWER_WATCHLIST_H
