#include "MemoryHistory.h"
#include "../CommandLineInterface.h"

bool ProcessMemoryViewer::History::IsEmpty() {
    return last_search->IsEmpty();
}

void ProcessMemoryViewer::History::Print() {
    last_search->Print();
}

ProcessMemoryViewer::History::History(ProcessMemoryViewer::VirtualMemoryWrapper &memoryWrapper) : memory_wrapper_(memoryWrapper){
}

void ProcessMemoryViewer::search_value::Print() {
    switch (type) {
        case VALUE_S8: {
            std::cout << value.s8;
        }
            break;
        case VALUE_U8: {
            std::cout << value.u8;
        }
            break;
        case VALUE_S16: {
            std::cout << value.s16;
        }
            break;
        case VALUE_U16: {
            std::cout << value.u16;
        }
            break;
        case VALUE_S32: {
            std::cout << value.s32;
        }
            break;
        case VALUE_U32: {
            std::cout << value.u32;
        }
            break;
        case VALUE_S64: {
            std::cout << value.s64;
        }
            break;
        case VALUE_U64: {
            std::cout << value.u64;
        }
            break;
        case VALUE_F32: {
            std::cout << value.f32;
        }
            break;
        case VALUE_F64: {
            std::cout << value.f64;
        }
            break;
    }
}

void ProcessMemoryViewer::search_value::PrintOriginal() {
    switch (type) {
        case VALUE_S8: {
            std::cout << original_value.s8;
        }
            break;
        case VALUE_U8: {
            std::cout << original_value.u8;
        }
            break;
        case VALUE_S16: {
            std::cout << original_value.s16;
        }
            break;
        case VALUE_U16: {
            std::cout << original_value.u16;
        }
            break;
        case VALUE_S32: {
            std::cout << original_value.s32;
        }
            break;
        case VALUE_U32: {
            std::cout << original_value.u32;
        }
            break;
        case VALUE_S64: {
            std::cout << original_value.s64;
        }
            break;
        case VALUE_U64: {
            std::cout << original_value.u64;
        }
            break;
        case VALUE_F32: {
            std::cout << original_value.f32;
        }
            break;
        case VALUE_F64: {
            std::cout << original_value.f64;
        }
            break;
    }
}

void ProcessMemoryViewer::MemoryList::Print() {
    if (!GetSize()) {
        std::cout << "No Results Found." << std::endl;
        return;
    }

    // Header
    std::cout << "Id\t\tAddress\t\t\tValue\t\tPrevious" << std::endl;

    // Contents
    for (int i = 0; i < GetSize(); i++) {
        void* addr = addresses.at(i);
        UpdateValue(addr);

        search_value v = values.at(i);
        search_value lv = last_values.at(i);

        std::cout << i << "\t\t" << addr << "\t\t";
        v.Print();
        std::cout << "\t\t";
        lv.Print();

        std::cout << std::endl;
    }
}

void ProcessMemoryViewer::MemoryList::PrintCount(){
    std::cout << "Found " << GetSize() << " matches. Type display to view." << std::endl;
}

ProcessMemoryViewer::MemoryList
ProcessMemoryViewer::MemoryList::GetChangedValues(ProcessMemoryViewer::search_value value, double eps) {
    MemoryList ml;
    ml.memory_wrapper = memory_wrapper;

    for (int i = 0; i < GetSize(); i++) {
        bool found = false;
        void *addr = addresses[i];
        value_type vtype = values[i].type;
        search_value new_value;
        UpdateValue(addr);

        switch (vtype) {
            case VALUE_S32: {
                int current_value = values[i].value.s32;
                int target_value = value.value.s32;
                if (abs(current_value - target_value) < eps) {
                    found = true;
                    new_value.type = vtype;
                    new_value.value.s32 = current_value;
                }
            }
                break;
            case VALUE_F32: {
                float current_value = values[i].value.f32;
                float target_value = value.value.s32;
                if (fabs(target_value - current_value) < eps) {
                    found = true;
                    new_value.type = vtype;
                    new_value.value.f32 = current_value;
                }
            }
                break;
            case VALUE_F64: {
                double current_value = values[i].value.f64;
                double target_value = value.value.f64;
                if (fabs(target_value - current_value) < eps) {
                    found = true;
                    new_value.type = vtype;
                    new_value.value.f64 = current_value;
                }
            }
                break;
            case VALUE_S64: {
                long current_value = values[i].value.s64;
                long target_value = value.value.s64;
                if (labs(target_value - current_value) < eps) {
                    found = true;
                    new_value.type = vtype;
                    new_value.value.s64 = current_value;
                }
            }
                break;
        }
        if (found) {
            ml.Add(addr, new_value, values[i]);
        }
    }
    return ml;
}

ProcessMemoryViewer::MemoryList
ProcessMemoryViewer::MemoryList::GetChangedValues(ProcessMemoryViewer::value_type type, double eps) {
    MemoryList ml;
    ml.memory_wrapper = memory_wrapper;

    for (int i = 0; i < GetSize(); i++) {
        void* addr = addresses[i];
        value_type vtype = values[i].type;
        search_value new_value;
        UpdateValue(addr);

        switch(vtype){
            case VALUE_S32: {
                int current_value = values[i].value.s32 ;
                int last_value = last_values[i].value.s32;
                if(abs(current_value - last_value) > eps){

                    new_value.type = vtype;
                    new_value.value.s32 = current_value;
                }
            } break;
            case VALUE_F32: {
                float current_value = values[i].value.f32;
                float old_value = last_values[i].value.f32;
                if(fabs(old_value - current_value) > eps){
                    new_value.type = vtype;
                    new_value.value.f32 = current_value;
                }
            } break;
            case VALUE_F64: {
                double current_value = values[i].value.f64;
                double old_value = last_values[i].value.f64;
                if(fabs(old_value - current_value) > eps){
                    new_value.type = vtype;
                    new_value.value.f64 = current_value;
                }
            } break;
            case VALUE_S64: {
                long current_value = values[i].value.s64;
                long old_value = last_values[i].value.s64;
                if(labs(old_value - current_value) > eps){
                    new_value.type = vtype;
                    new_value.value.s64 = current_value;
                }
            } break;
        }
        ml.Add(addr, new_value, last_values[i]);
    }

    return ml;
}

//ProcessMemoryViewer::MemoryList::MemoryList(ProcessMemoryViewer::VirtualMemoryWrapper *memoryWrapper) : memory_wrapper(
//        memoryWrapper) {}

void ProcessMemoryViewer::MemoryList::UpdateValue(void *addr) {
    if(!memory_wrapper) {
        return;
    }
    int i = GetIndex(addr);
    search_value value = values[i];
    value_type vtype = value.type;

    switch(vtype){
        case VALUE_S32: {
            int32_t current_value = memory_wrapper->Read<int>(addr);
            last_values[i].value.s32 = value.value.s32;
            values[i].value.s32 = current_value;
        } break;
        case VALUE_F32: {
            int32_t current_value = memory_wrapper->Read<float>(addr);
            last_values[i].value.f32 = value.value.f32;
            values[i].value.s32 = current_value;
        } break;
        case VALUE_F64: {
            int32_t current_value = memory_wrapper->Read<double>(addr);
            last_values[i].value.f64 = value.value.f64;
            values[i].value.s64 = current_value;
        } break;
        case VALUE_S64: {
            int32_t current_value = memory_wrapper->Read<long>(addr);
            last_values[i].value.s64 = value.value.s64;
            values[i].value.s64 = current_value;
        } break;
    }

}

ProcessMemoryViewer::MemoryList::MemoryList() {}


//ProcessMemoryViewer::MemoryList::MemoryList(const ProcessMemoryViewer::VirtualMemoryWrapper wrapper) : memory_wrapper(
//        const_cast<VirtualMemoryWrapper &>(wrapper)) {
//
//}
