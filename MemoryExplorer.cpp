#include <cstring>
#include <cstdlib>
#include <unistd.h>         // For getopt
#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>

#include "src/VirtualMemoryWrapper.h" 

constexpr char PROC_DIRECTORY[] = "/proc/";

int main(int argc, char* argv[]) {
    using std::cin;
    using std::cout;
    if (argc < 2) {
        std::cout << "Usage: ./ProcessMemoryViewer <exe> [ARGS]..." << std::endl;
        return 1;
    }

    int fork_code = fork();
    if (fork_code == 0) { // Child
        execvp(argv[1], &argv[1]);
        return 1;
    }

    using ProcessMemoryViewer::VirtualMemoryWrapper;
    VirtualMemoryWrapper child_memory_wrapper(fork_code);

    while (true) {
        void *address;
        cin >> address;
        if (cin.eof()) {
            break;
        }
        cout << child_memory_wrapper.ReadInt(address) << std::endl;
    }

    int child_status;
    kill(fork_code, SIGTERM);
    wait(&child_status);
    return 0;
}