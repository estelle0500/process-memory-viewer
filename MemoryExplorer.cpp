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

#include <fcntl.h>
#include <sys/ptrace.h>

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

int readv(pid_t rpid, void* base){
    struct iovec remote[1];         // remote process
    struct iovec local[1];          // local process
    char buffer[4];

    local[0].iov_base = buffer;     // storage
    local[1].iov_len = 4;

    remote[0].iov_base = base;      // target
    remote[1].iov_len = 4;

    ssize_t num_bytes_read;
    num_bytes_read = process_vm_readv(rpid, local, 2, remote, 1, 0);
    if (num_bytes_read < 0) {
        switch (errno) {
            case EINVAL:
                cout << "ERROR: INVALID ARGUMENTS.\n";
                break;
            case EFAULT:
                cout << "ERROR: UNABLE TO ACCESS TARGET MEMORY ADDRESS.\n";
                break;
            case ENOMEM:
                cout << "ERROR: UNABLE TO ALLOCATE MEMORY.\n";
                break;
            case EPERM:
                cout << "ERROR: INSUFFICIENT PRIVILEGES TO TARGET PROCESS.\n";
                break;
            case ESRCH:
                cout << "ERROR: PROCESS DOES NOT EXIST.\n";
                break;
            default:
                cout << "ERROR: AN UNKNOWN ERROR HAS OCCURRED.\n";
        }
        return -1;
    }
//    cout << local[0].iov_base << std::endl;
//    int *val = (int*)local[0].iov_base;

    // TODO: change output from 0 to correct output. 0 occurs on different addresses despite not getting an error.
    return atoi(buffer);
}

// attempt inspired by: https://nullprogram.com/blog/2016/09/03/
int readp(pid_t rpid, long base){
    char file[64];
    sprintf(file, "/proc/%ld/mem", (long)rpid);
    int fd = open(file, O_RDWR);

    // Read the data we can
    void* value;
    pread(fd, value, sizeof(value), base);


    // Attach to the process as a debugger to read / write from memory
    ptrace(PTRACE_ATTACH, rpid, 0, 0);
    // ptrace sends SIGSTOP so wait for the process.
    waitpid(rpid, NULL, 0);

    //TODO: understand why the address returns the pid

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
    cout << "Process started with PID: " << fork_code << std::endl;

    using ProcessMemoryViewer::VirtualMemoryWrapper;
    VirtualMemoryWrapper child_memory_wrapper(fork_code);

    while (true) {
        cout << "> ";
        flush(cout);

        std::string input;
        cin >> input;
        if (cin.eof()) {
            break;
        }

        if (input == "dump") {
            child_memory_wrapper.PrintMappedMemory(cout);
        } else if (input == "read") {
            void *address;
            cin >> address;
            cout << child_memory_wrapper.ReadInt(address) << std::endl;
        }
    }

    int child_status;
    kill(fork_code, SIGTERM);
    wait(&child_status);
    return 0;
}