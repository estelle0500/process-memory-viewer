//
// Created by tyler on 11/6/20.
//

#include "Watchlist.h"
#include <algorithm>

namespace ProcessMemoryViewer {


    void Watchlist::Print() {
        for (int i = 0; i < addresses_.size(); ++i) {
            PrintAddress(i);
        }
    }

    void Watchlist::PrintChanged() {
        for (int i = 0; i < addresses_.size(); ++i) {

        }
    }

    int Watchlist::GetSize() {
        return addresses_.size();
    }

    void Watchlist::Add(void* addr) {
        std::vector<void*>::iterator it = std::find(addresses_.begin(), addresses_.end(), addr);
        if (it != addresses_.end()){
            return;
        }
        int value = memory_wrapper_.Read<int>(addr);
        original_values_.push_back(value);
        addresses_.push_back(addr);
    }

    void Watchlist::PrintAddress(int id) {
        void* addr = addresses_.at(id);
        int value = memory_wrapper_.Read<int>(addr);
        int old_value = original_values_.at(id);
        char change = '=';
        if(value > old_value){
            change = '+';
        } else if (value < old_value){
            change = '-';
        }
        std::cout << addr << "\t\t\t" << "(" << change << ") " << value << std::endl;
    }
}