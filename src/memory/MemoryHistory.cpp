#include "MemoryHistory.h"
#include "../CommandLineInterface.h"

bool ProcessMemoryViewer::History::IsEmpty() {
    return last_search.IsEmpty();
}

ProcessMemoryViewer::MemoryList ProcessMemoryViewer::History::GetChangedValues(ProcessMemoryViewer::value_type type, double eps) {
    MemoryList ml;

    for (int i = 0; i < last_search.GetSize(); ++i) {
        void* addr = last_search.addresses[i];
        value_type vtype = last_search.values[i].type;
        search_value searchValue = last_search.values[i];
        switch(vtype){
            case VALUE_S32: {
                int32_t old_value = searchValue.value.s32;
                int32_t current_value = memory_wrapper_.Read<int32_t>(addr);
                searchValue.value.s32 = current_value;
                if(abs(old_value - current_value) > eps){
                    search_value new_value;

                    new_value.type = vtype;
                    new_value.value.s32 = current_value;

                    ml.Add(addr, new_value);
                }
            } break;
            case VALUE_F32: {
                float old_value = searchValue.value.f32;
                float current_value = memory_wrapper_.Read<float>(addr);
                searchValue.value.f32 = current_value;
                if(fabs(old_value - current_value) > eps){
                    search_value new_value;

                    new_value.type = vtype;
                    new_value.value.f32 = current_value;

                    ml.Add(addr, new_value);
                }
            } break;
            case VALUE_F64: {
                double old_value = searchValue.value.f64;
                double current_value = memory_wrapper_.Read<double>(addr);
                searchValue.value.f64 = current_value;
                if(fabs(old_value - current_value) > eps){
                    search_value new_value;

                    new_value.type = vtype;
                    new_value.value.f32 = current_value;

                    ml.Add(addr, new_value);
                }
            } break;
        }
    }

    last_filtered_search = ml;
    return ml;
}

ProcessMemoryViewer::MemoryList
ProcessMemoryViewer::History::GetChangedValues(ProcessMemoryViewer::search_value value, double eps) {
    MemoryList ml;

    for (int i = 0; i < last_search.GetSize(); ++i) {
        void* addr = last_search.addresses[i];
        value_type vtype = last_search.values[i].type;
        search_value searchValue = last_search.values[i];
        switch(vtype){
            case VALUE_S32: {
                int32_t old_value = searchValue.value.s32;
                int32_t current_value = memory_wrapper_.Read<int32_t>(addr);
                searchValue.value.s32 = current_value;
                if(abs(value.value.s32 - current_value) < eps){
                    search_value new_value;

                    new_value.type = vtype;
                    new_value.value.s32 = current_value;

                    ml.Add(addr, new_value);
                }
            } break;
            case VALUE_F32: {
                float old_value = searchValue.value.f32;
                float current_value = memory_wrapper_.Read<float>(addr);
                searchValue.value.f32 = current_value;
                if(fabs(old_value - current_value) < eps){
                    search_value new_value;

                    new_value.type = vtype;
                    new_value.value.f32 = current_value;

                    ml.Add(addr, new_value);
                }
            } break;
            case VALUE_F64: {
                double old_value = searchValue.value.f64;
                double current_value = memory_wrapper_.Read<double>(addr);
                searchValue.value.f64 = current_value;
                if(fabs(old_value - current_value) < eps){
                    search_value new_value;

                    new_value.type = vtype;
                    new_value.value.f32 = current_value;

                    ml.Add(addr, new_value);
                }
            } break;
        }
    }

    last_filtered_search = ml;
    return ml;
}

void ProcessMemoryViewer::History::Print() {
    /* Display the contents of results */
    if (!last_search.GetSize()) {
        std::cout << "No Results Found." << std::endl;
        return;
    }

    // Header
    std::cout << "Id\t\tAddress\t\t\tValue\t\tPrevious" << std::endl;

    // Contents
    for (int i = 0; i < last_search.GetSize(); ++i) {
        search_value v = last_search.values.at(i);
        void* addr = last_search.addresses.at(i);
        std::cout << i << "\t\t" << addr << "\t\t";
        v.value.s32 = memory_wrapper_.Read<int32_t>(addr);
        v.Print();
        std::cout << "\t\t";
        v.PrintOriginal();

        std::cout << std::endl;
    }

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
    /* Display the contents of results */
    if (!GetSize()) {
        std::cout << "No Results Found." << std::endl;
        return;
    }

    // Header
    std::cout << "Id\t\tAddress\t\t\tValue\t\tPrevious" << std::endl;

    // Contents
    for (int i = 0; i < GetSize(); ++i) {
        search_value v = values.at(i);
        void* addr = addresses.at(i);
        std::cout << i << "\t\t" << addr << "\t\t";
        v.Print();
        std::cout << "\t\t";
        v.PrintOriginal();

        std::cout << std::endl;
    }
}
