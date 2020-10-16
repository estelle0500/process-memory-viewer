cmake_minimum_required(VERSION 3.17)
project(ProcessMemoryViewer)

set(CMAKE_CXX_STANDARD 20)

list(APPEND SOURCE_DIR_FILES "src/VirtualMemoryWrapper.cpp")

add_executable(ProcessMemoryViewer MemoryExplorer.cpp ${SOURCE_DIR_FILES})