#include <cstring>
#include <cstdlib>
#include <unistd.h>         // For getopt
#include <dirent.h>         // For DIR
#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>
#include <sys/uio.h>        // For process_vm_readv

#include "src/VirtualMemoryWrapper.h" 

constexpr char PROC_DIRECTORY[] = "/proc/";
using std::string;
using std::cin;
using std::cout;

pid_t get_pid_from_name(std::string procName)
{
    pid_t pid = -1;
    DIR *proc_dir = opendir(PROC_DIRECTORY);

    if (proc_dir){
        struct dirent *proc;
        while (pid < 0 && (proc = readdir(proc_dir)))
        {
            // Skip non-numeric entries
            int id = atoi(proc->d_name);
            if (id > 0)
            {
                string cmdPath = string(PROC_DIRECTORY) + proc->d_name + "/cmdline";
                std::ifstream cmdFile(cmdPath.c_str());
                string line;
                getline(cmdFile, line);
                if (!line.empty()){

                    size_t pos = line.find('\0');
                    if (pos != -1){
                        line = line.substr(0, pos);
                    }

                    size_t nopath_pos = line.rfind('/');
                    if (pos != -1){
                        line = line.substr(nopath_pos + 1);
                    }

                    if (procName == line){
                        pid = id;
                    }
                }
            }
        }
    }

    closedir(proc_dir);

    return pid;
}


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

    using ProcessMemoryViewer::VirtualMemoryWrapper;
    VirtualMemoryWrapper child_memory_wrapper(fork_code);

    while (true) {
        void* input;
        cin >> input;

        if (cin.eof()) {
            break;
        }
        cout << child_memory_wrapper.ReadInt(input) << std::endl;
    }

    int child_status;
    kill(fork_code, SIGTERM);
    wait(&child_status);
    return 0;
}