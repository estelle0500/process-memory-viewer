#include "CommandLineInterface.h"

#include <dirent.h>         // For DIR
#include <sys/types.h>
#include <signal.h>
#include <fstream>

namespace ProcessMemoryViewer {
static constexpr char PROC_DIRECTORY[] = "/proc/";
using std::string;

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

void set_proc_run_state(pid_t pid, bool running = true){
    kill(pid, running ? SIGCONT : SIGSTOP);
}

void CommandLineInterface::HandleInput(std::string input) {
    std::istringstream input_stream(input);
    std::string command;
    input_stream >> command;

    if (command == "info") {
        memory_wrapper_.PrintRegionInfo(out_stream_);
    } else if (command == "cont") {
        set_proc_run_state(memory_wrapper_.process_id(), true);
    } else if (command == "getpid") {
        std::string name;
        input_stream >> name;
        out_stream_ << get_pid_from_name(name) << std::endl;
    } else if (command == "getregion") {
        void *address;
        input_stream >> address;
        ProcessMemoryViewer::MemoryRegion *mr = memory_wrapper_.GetRegionOfAddress(address);
        out_stream_ << "Region: " << mr->id << std::endl;
    } else if (command == "pause") {
        set_proc_run_state(memory_wrapper_.process_id(), false);
    } else if (command == "printregion") {
        int region;
        input_stream >> region;
        out_stream_ << "Printing region: " << region << std::endl;
        memory_wrapper_.PrintRegion(region, 4);
    } else if (command == "printregions") {
        memory_wrapper_.PrintRegionBounds();
    } else if (command == "read") {
        void *address;
        input_stream >> address;
        out_stream_ << memory_wrapper_.Read<int>(address) << std::endl;
    } else if (command == "findint" || command == "find") {
        int value;
        input_stream >> value;
        memory_wrapper_.FindValues<int>(value);
    } else if (command == "findfloat") {
        float value;
        input_stream >> value;
        memory_wrapper_.FindValues<float>(value);
    } else if (command == "findpattern") {
        char *data, *pattern;
        input_stream >> data >> pattern;
        out_stream_ << "Find 1: " << memory_wrapper_.FindPattern(data, pattern) << std::endl;
    } else if (command == "kill" || command == "exit") {
        kill(memory_wrapper_.process_id(), SIGTERM);
        out_stream_ << "Child process has been terminated.\n" << std::endl;
        exit(1);
    } else if (command == "write") {
        void *address;
        int value;
        input_stream >> address >> value;
        memory_wrapper_.Write<int>(address, value);  
    } else {
        out_stream_ << "Unrecognized command" << std::endl;
    }
}
} // namespace ProcessMemoryViewer