#pragma once

#include <iostream>
#include <sstream>
#include <vector>

#include "VirtualMemoryWrapper.h"

namespace ProcessMemoryViewer {
class CommandLineInterface {
  public:
    CommandLineInterface(VirtualMemoryWrapper &memory_wrapper, std::ostream &out_stream)
        : memory_wrapper_(memory_wrapper), out_stream_(out_stream) {}

    void HandleInput(std::string input);

  private:
    VirtualMemoryWrapper &memory_wrapper_;
    std::ostream &out_stream_;
};
} // namespace ProcessMemoryViewer