cmake_minimum_required(VERSION 3.16)
project(process-memory-viewer)

set(CMAKE_CXX_STANDARD 20)

list(APPEND SOURCE_DIR_FILES "src/memory/VirtualMemoryWrapper.cpp"
                             "src/memory/MemoryRegion.cpp"
                             "src/memory/MemorySnapshot.cpp"
                             "src/CommandLineInterface.cpp"
                             "src/Watchlist.cpp"
                             "src/ProcessTracer.cpp")

add_executable(sequence tests/sequence.cpp)
add_executable(counter tests/counter.cpp)
add_executable(types tests/types.cpp)
add_executable(infinite tests/infinite_loop.cpp)
add_executable(process-memory-viewer MemoryExplorer.cpp ${SOURCE_DIR_FILES})


# add_executable(process-memory-viewer MemoryExplorer.cpp ${SOURCE_DIR_FILES} tests/counter.cpp)