#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>

#include "src/memory/VirtualMemoryWrapper.h"
#include "src/Watchlist.h"
#include "src/CommandLineInterface.h"

using std::string;
using std::cin;
using std::cout;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./ProcessMemoryViewer <exe> [ARGS]..." << std::endl;
        return 1;
    }

    int fork_code = fork();
    if (fork_code == 0) { // Child
        execvp(argv[1], &argv[1]);
        return 1;
    }
    cout << "Process started with PID: " << fork_code << std::endl;

    using namespace ProcessMemoryViewer;
    VirtualMemoryWrapper child_memory_wrapper(fork_code);
    Watchlist watchlist(child_memory_wrapper);
    CommandLineInterface cli(child_memory_wrapper, cout, (Watchlist &) watchlist);

    while (true) {
        cout << "> ";
        flush(cout);

        std::string input;
        getline(cin, input);
        if (cin.eof()) {
            break;
        }

        cli.HandleInput(input);
    }

    int child_status;
    kill(fork_code, SIGTERM);
    wait(&child_status);
    return 0;
}