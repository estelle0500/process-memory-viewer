#include <vector>
#include "VirtualMemoryWrapper.h"
#include <memory>

#ifndef PROCESS_MEMORY_VIEWER_WATCHLIST_H
#define PROCESS_MEMORY_VIEWER_WATCHLIST_H

/* Saves a list of addresses to monitor addresses of a given type */
namespace ProcessMemoryViewer {

    /* Interface class to create a vector. */
    class IAddress {

    };

    /* */
    template<typename T>
    class Address : public IAddress {
    public:
        void* addresses;
        T original_value;
        T last_value;

        /* Get the value of the address */
        T GetValue();

        /* Get the delta of the current and original values */
        T GetChange();
    };

    /* */
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
        /* Format a single address*/
        void PrintAddress(int id);
        VirtualMemoryWrapper &memory_wrapper_;
        std::vector<void*> addresses_;
        std::vector<int> original_values_;
    };
}

#endif //PROCESS_MEMORY_VIEWER_WATCHLIST_H
