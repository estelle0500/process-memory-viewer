#pragma once

#include <vector>
#include <unordered_map>
#include <cstring>

#include "MemoryRegion.h"
#include "VirtualMemoryWrapper.h"

namespace ProcessMemoryViewer {

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

    struct search_value {
        enum value_type type = VALUE_S32;
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

        /* Copy a byte array to a value*/
        void SetValue(char val[]){
            switch(this->type){
                case VALUE_S8: {
                    std::memcpy(val,&value.s8,sizeof(value.s8));
                }
                    break;
                case VALUE_U8: {
                    std::memcpy(val,&value.u8,sizeof(value.u8));
                }
                    break;
                case VALUE_S16: {
                    std::memcpy(val,&value.s16,sizeof(value.s16));
                }
                    break;
                case VALUE_U16: {
                    std::memcpy(val,&value.u16,sizeof(value.u16));
                }
                    break;
                case VALUE_S32: {
                    std::memcpy(val,&value.s32,sizeof(value.s32));
                }
                    break;
                case VALUE_U32: {
                    std::memcpy(val,&value.u32,sizeof(value.u32));
                }
                    break;
                case VALUE_S64: {
                    std::memcpy(val,&value.s64,sizeof(value.s64));
                }
                    break;
                case VALUE_U64: {
                    std::memcpy(val,&value.u64,sizeof(value.u64));
                }
                    break;
                case VALUE_F32: {
                    std::memcpy(val,&value.f32,sizeof(value.f32));
                }
                    break;
                case VALUE_F64: {
                    std::memcpy(val,&value.f64,sizeof(value.f64));
                }
                    break;
            }
        }
    };

    class Results {
    public:
        std::vector<void *> addresses;
        std::vector<search_value> values;

        int bytes = 4;

        /* Returns the size of the vectors which should not be different. */
        int GetSize() { return addresses.size(); };

        /* Checks if there are any results */
        bool IsEmpty() { return values.empty() || addresses.empty(); }

        /* Gets the stored value based on an index from the address */
        search_value GetValue(void *addr){
            auto it = std::find(addresses.begin(), addresses.end(), addr);
            if(it == addresses.end()){
                return *new search_value();
            }
            int index = it - addresses.begin();
            return values.at((size_t) index);
        }

        /* Display the contents of results */
        void Print(){
            if(!GetSize()){
                std::cout << "No Results Found." << std::endl;
                return;
            }

            // Header
            std::cout << "Id\t\tAddress\t\t\tValue" << std::endl;

            // Contents
            for (int i = 0; i < GetSize(); ++i) {
                search_value v = values.at(i);
                std::cout << i << "\t\t" << addresses.at(i) << "\t\t\t";
                switch (v.type) {
                    case VALUE_S8: {
                        std::cout << v.value.s8;
                    }
                        break;
                    case VALUE_U8: {
                        std::cout << v.value.u8;
                    }
                        break;
                    case VALUE_S16: {
                        std::cout << v.value.s16;
                    }
                        break;
                    case VALUE_U16: {
                        std::cout << v.value.u16;
                    }
                        break;
                    case VALUE_S32: {
                        std::cout << v.value.s32;
                    }
                        break;
                    case VALUE_U32: {
                        std::cout << v.value.u32;
                    }
                        break;
                    case VALUE_S64: {
                        std::cout << v.value.s64;
                    }
                        break;
                    case VALUE_U64: {
                        std::cout << v.value.u64;
                    }
                        break;
                    case VALUE_F32: {
                        std::cout << v.value.f32;
                    }
                        break;
                    case VALUE_F64: {
                        std::cout << v.value.f64;
                    }
                        break;
                }
                std::cout << std::endl;
            }
        }
    };


/* A snapshot contains the memory state at one point in time */
class MemorySnapshot {
  public:
    /* Save a snapshot of current process memory */
    MemorySnapshot(const VirtualMemoryWrapper &memory_wrapper);

    /* Prints all addresses that have a different value between this and other snapshot 
       Differentiates between added, deleted, and modified addresses */
    void PrintAddressDifferences(const MemorySnapshot &other_snapshot) const;

    /* Search addresses for "value" */
    template<typename T>
    Results SearchValue(T value, double eps, value_type type) {
        Results res;

        for (const auto &page_data : memory_) {
            char *base_addr = (char*) page_data.first;

            for (size_t offset = 0; offset < PAGE_SIZE; offset += sizeof(value)) {
                T val = *(T*) &page_data.second[offset];

                if ((std::is_floating_point<T>::value && fabs(val - value) < eps)
                    || val == value) {
                    void *addr = base_addr + offset;
                    res.addresses.push_back(addr);

                    search_value v;
                    v.type = type;
                    switch(v.type){
                        case VALUE_S32: {
                            v.value.s32 = val;
                        } break;
                        case VALUE_F32: {
                            v.value.f32 = val;
                        } break;
                        case VALUE_F64: {
                            v.value.f64 = val;
                        } break;
                    }

                    res.values.push_back(v);
                }
            }
        }

        last_result = &res;
        res.Print();
        return res;
    }

    /* Get the previously outputted addresses */
    std::vector<void *> GetLastAddresses(){return last_result->addresses;};

    /* Get the previously outputted values */
    std::vector<search_value> GetLastValues(){return last_result->values;};

    template<typename T>
    Results SearchValue(T value) {
        return SearchValue(value, 0.0);
    }

    Results *last_result;
  private:
    std::unordered_map<void *, std::vector<unsigned char> > memory_;
    size_t PAGE_SIZE;
};

/* Container for many snapshots */
class MemorySnapshotManager {
  public:
    /* Save a snapshot of current process memory 
       Returns an id that can be used to refer to the snapshot */
    unsigned int SaveSnapshot(const VirtualMemoryWrapper &memory_wrapper);

    /* Remove a snapshot given the current id */
    void DeleteSnapshot(unsigned int id);

    /* Returns the amount of snapshots saved */
    int GetSize();

    /* Compare old and new snapshots */
    void PrintComparison(unsigned int old_snapshot_id, unsigned int new_snapshot_id);

  private:
    std::vector<MemorySnapshot> snapshots_;
};
} // namespace ProcessMemoryViewer