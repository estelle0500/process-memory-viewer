#include <string>
#include <iostream>

#include "src/memory/VirtualMemoryWrapper.h"
#include "src/Watchlist.h"
#include "src/CommandLineInterface.h"
#include "src/ProcessTracer.h"

using std::string;
using std::cin;
using std::cout;
using std::endl;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: ./ProcessMemoryViewer <exe> [ARGS]..." << endl;
        return 1;
    }

    using namespace ProcessMemoryViewer;
    ProcessTracer tracer;

    tracer.Start(argv[1], argv + 1);

    VirtualMemoryWrapper child_memory_wrapper(tracer.pid());
    Watchlist watchlist(child_memory_wrapper);
    History history(child_memory_wrapper);
    CommandLineInterface cli(child_memory_wrapper, watchlist, tracer, history);

    while (true) {
        cout << "> ";
        flush(cout);

        std::string input;
        if (getline(cin, input).eof()) {
            break;
        }

        cli.HandleInput(input);
    }

    cout << "Exiting..." << endl;
    return 0;
}