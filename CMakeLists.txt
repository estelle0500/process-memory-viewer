cmake_minimum_required(VERSION 3.16)
project(process-memory-viewer)

set(CMAKE_CXX_STANDARD 20)

list(APPEND SOURCE_DIR_FILES "src/VirtualMemoryWrapper.cpp"
                             "src/MemoryRegion.cpp"
                             "src/CommandLineInterface.cpp")

add_executable(sequence tests/sequence.cpp)
add_executable(counter tests/counter.cpp)
add_executable(process-memory-viewer MemoryExplorer.cpp ${SOURCE_DIR_FILES})


# add_executable(process-memory-viewer MemoryExplorer.cpp ${SOURCE_DIR_FILES} tests/counter.cpp)