#ifndef PROCESS_MEMORY_VIEWER_MEMORYHISTORY_H
#define PROCESS_MEMORY_VIEWER_MEMORYHISTORY_H

#include <memory>
#include <cstring>
#include "VirtualMemoryWrapper.h"

namespace ProcessMemoryViewer {

    /* The type of value the memory is read as */
    enum value_type {
        VALUE_S8,
        VALUE_U8,
        VALUE_S16,
        VALUE_U16,
        VALUE_S32,
        VALUE_U32,
        VALUE_S64,
        VALUE_U64,
        VALUE_F32,
        VALUE_F64,
    };

    /* Data as numerous types */
    struct search_value {
        enum value_type type = VALUE_S64;

        union {
            int8_t s8;
            uint8_t u8;
            int16_t s16;
            uint16_t u16;
            int32_t s32;
            uint32_t u32;
            int64_t s64;
            uint64_t u64;
            float f32;
            double f64;
        } value;

        union {
            int8_t s8;
            uint8_t u8;
            int16_t s16;
            uint16_t u16;
            int32_t s32;
            uint32_t u32;
            int64_t s64;
            uint64_t u64;
            float f32;
            double f64;
        } original_value;

        /* Print a search value to cout */
        void Print();

        void PrintOriginal();
    };

    /* A list of constant addresses & values taken at a certain interval */
    class MemoryList {
    public:
        std::vector<void *> addresses;
        std::vector<search_value> values;

        /* Returns the size of the vectors which should not be different. */
        int GetSize() { return addresses.size(); };

        /* Checks if there are any results */
        bool IsEmpty() { return &(addresses) < (void *) 0x2000000 || values.empty() || addresses.empty(); }

        void Empty() {
            addresses.erase(addresses.begin());
            values.erase(values.begin());
        }

        /* Gets the stored value based on an index from the address */
        search_value GetValue(void *addr) {
            auto it = std::find(addresses.begin(), addresses.end(), addr);
            if (it == addresses.end()) {
                return *new search_value();
            }
            int index = it - addresses.begin();
            return values.at((size_t) index);
        }


        /* Helper method to add an address & value combination */
        void Add(void *addr, search_value value) {
            addresses.push_back(addr);
            values.push_back(value);
        }

        /* Print the list with the original value as it is stored */
        void Print();
    };

    /* History of read & modified addresses that can be recalled for ease of use */
    class History {
    public:
        /* Last list to be interacted with. Shortcut for adding to watchlist */
        MemoryList *last_list;

        /* Original value search */
        MemoryList last_search;

        /* A search with a filter applied (ie: changed value, next value)*/
        MemoryList last_filtered_search;

        /* Address of the last write */
        void *last_modified_address;

        /* Search History is empty */
        bool IsEmpty();

        /* Get a list all values that have changed from the last search*/
        MemoryList GetChangedValues(value_type type, double eps);

        /* Get a list of all values from the last search that now match a specified value */
        MemoryList GetChangedValues(search_value value, double eps);

        /* Print the values of the last search */
        void Print();

        History(VirtualMemoryWrapper &memoryWrapper)
                : memory_wrapper_(memoryWrapper) {}


    private:
        VirtualMemoryWrapper &memory_wrapper_;
    };
}


#endif //PROCESS_MEMORY_VIEWER_MEMORYHISTORY_H
