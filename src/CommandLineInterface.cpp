#include "CommandLineInterface.h"

#include <dirent.h>         // For DIR
#include <sys/types.h>
#include <signal.h>
#include <fstream>

namespace ProcessMemoryViewer {
static constexpr char PROC_DIRECTORY[] = "/proc/";
using std::string;

pid_t get_pid_from_name(std::string procName) {
    DIR *proc_dir = opendir(PROC_DIRECTORY);
    if (!proc_dir) {
        perror("Could not open proc directory");
        return -1;
    }

    pid_t pid = -1;
    struct dirent *proc;
    while ((proc = readdir(proc_dir))) {
        // Skip non-numeric entries
        int id = atoi(proc->d_name);
        if (id < 0) {
            continue;
        }

        string cmdPath = string(PROC_DIRECTORY) + proc->d_name + "/cmdline";
        std::ifstream cmdFile(cmdPath.c_str());
        string line;
        getline(cmdFile, line);

        size_t pos = line.find('\0');
        if (pos != string::npos) {
            line = line.substr(0, pos);
        }

        if (procName == line) {
            pid = id;
            break;
        }
    }

    closedir(proc_dir);
    return pid;
}

void set_proc_run_state(pid_t pid, bool running = true) {
    kill(pid, running ? SIGCONT : SIGSTOP);
}

void CommandLineInterface::HandleInput(std::string input) {
    std::istringstream input_stream(input);
    std::string command;
    input_stream >> command;
    memory_wrapper_.ParseMaps();

    if (command == "info") {
        memory_wrapper_.PrintRegionInfo();
    } else if (command == "cont") {
        set_proc_run_state(memory_wrapper_.process_id(), true);
    } else if (command == "getpid") {
        std::string name;
        input_stream >> name;
        out_stream_ << get_pid_from_name(name) << std::endl;
    } else if (command == "getregion") {
        void *address;
        input_stream >> address;
        const ProcessMemoryViewer::MemoryRegion &mr = memory_wrapper_.GetRegionOfAddress(address);
        out_stream_ << "Region: " << mr.id << std::endl;
    } else if (command == "pause") {
        set_proc_run_state(memory_wrapper_.process_id(), false);
    } else if (command == "setep") {
        double ep;
        input_stream >> ep;
        memory_wrapper_.set_ep(ep);
    } else if (command == "printregion" || command == "region") {
        int region;
        input_stream >> region;
        out_stream_ << "Printing region: " << region << std::endl;
        memory_wrapper_.PrintRegion(region, 4);
    } else if (command == "read") {
        void *address;
        input_stream >> address;
        out_stream_ << memory_wrapper_.Read<int>(address) << std::endl;
    } else if (command == "findint" || command == "find") {
        int value;
        input_stream >> value;
        memory_wrapper_.SearchValue<int>(value);
    } else if (command == "findfloat") {
        float value;
        input_stream >> value;
        memory_wrapper_.SearchValue<float>(value);
    } else if (command == "finddouble") {
        float value;
        input_stream >> value;
        memory_wrapper_.SearchValue<double>(value);
    } else if (command == "kill" || command == "exit") {
        kill(memory_wrapper_.process_id(), SIGTERM);
        out_stream_ << "Child process has been terminated.\n" << std::endl;
        exit(1);
    } else if (command == "writeint" || command == "write") {
        void *address;
        int value;
        input_stream >> address >> value;
        memory_wrapper_.Write<int>(address, value);
    } else if (command == "snapshot") {
        unsigned int snapshot_id = snapshot_manager_.SaveSnapshot(memory_wrapper_);
        out_stream_ << "Saved snapshot with id " << snapshot_id << std::endl;
    } else if (command == "deletesnapshot"){
        unsigned int snapshot_id;
        input_stream >> snapshot_id;
        snapshot_manager_.DeleteSnapshot(snapshot_id);
        out_stream_ << "Removed snapshot with id " << snapshot_id << std::endl;
    } else if (command == "compare") {
        unsigned int old_snapshot_id, new_snapshot_id;
        input_stream >> old_snapshot_id >> new_snapshot_id;

        bool compare_current = (!new_snapshot_id);
        if(compare_current){
            if(!snapshot_manager_.GetSize()){
                out_stream_ << "Use 'snapshot' to save a snapshot to compare. " << std::endl;
                return;
            }
            new_snapshot_id = snapshot_manager_.SaveSnapshot(memory_wrapper_);
        }

        snapshot_manager_.PrintComparison(old_snapshot_id, new_snapshot_id);

        if(compare_current){
            snapshot_manager_.DeleteSnapshot(new_snapshot_id);
            new_snapshot_id = NULL;
        }
    } else {
        out_stream_ << "Unrecognized command" << std::endl;
    }
}
} // namespace ProcessMemoryViewer