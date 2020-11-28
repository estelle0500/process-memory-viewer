#include "CommandLineInterface.h"
#include "memory/MemoryHistory.h"

#include <dirent.h>         // For DIR
#include <sys/types.h>
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

void CommandLineInterface::HandleInput(std::string input) {
    using std::cout;
    using std::endl;

    std::istringstream input_stream(input);
    std::string command;
    input_stream >> command;

    if (!tracer_.IsRunning()) {
        printf("Child process has stopped\n");
        exit(0);
    }
    memory_wrapper_.ParseMaps();
    MemorySnapshot current(memory_wrapper_);

    if (command == "info") {
        memory_wrapper_.PrintRegionInfo();
    } else if (command == "cont") {
        tracer_.Continue();
    } else if (command == "getpid") {
        std::string name;
        input_stream >> name;
        cout << get_pid_from_name(name) << std::endl;
    } else if (command == "getregion") {
        void *address;
        input_stream >> address;
        const ProcessMemoryViewer::MemoryRegion &mr = memory_wrapper_.GetRegionOfAddress(address);
        cout << "Region: " << mr.id << std::endl;
    } else if (command == "pause") {
        tracer_.Pause();
    } else if (command == "setep") {
        input_stream >> eps_;
    } else if (command == "printregion" || command == "region") {
        int region;
        input_stream >> region;
        cout << "Printing region: " << region << std::endl;
        memory_wrapper_.PrintRegion(region, 4);
    } else if (command == "read") {
        void *address;
        input_stream >> address;
        cout << memory_wrapper_.Read<int>(address) << std::endl;
    } else if (command == "findint" || command == "find") {
        int value;
        input_stream >> value;
        MemoryList ml = current.SearchValue<int>(value, eps_, VALUE_S32);
        ml.Print();
        if(!ml.IsEmpty()){
            history_.last_search = ml;
        }
    } else if (command == "findfloat") {
        float value;
        input_stream >> value;

        MemoryList ml = current.SearchValue<float>(value, eps_, VALUE_F32);
        ml.Print();
        if(!ml.IsEmpty()){
            history_.last_search = ml;
        }
    } else if (command == "finddouble") {
        float value;
        input_stream >> value;

        MemoryList ml = current.SearchValue<double>(value, eps_, VALUE_F64);
        ml.Print();
        if(!ml.IsEmpty()){
            history_.last_search = ml;
        }
    } else if (command == "findstr") {
        // Converts rest of the stream (except leading whitespace but including spaces between words) to a string
        input_stream.ignore();
        string pattern(std::istreambuf_iterator<char>(input_stream), {}); 

        auto matches = current.SearchString(pattern);
        for (auto match : matches) {
            cout << match << std::endl;
            cout << memory_wrapper_.GetRegionOfAddress(match) << std::endl;
        }
    } else if (command == "kill" || command == "exit") {
        tracer_.Kill();
        exit(0);
    } else if (command == "last" || command == "history") {
        if(command == input){
            history_.Print();
            return;
        }
        if(input.find("changed") != std::string::npos){
            if(history_.IsEmpty()){
                cout << "No history to search" << endl;
                return;
            }
            history_.GetChangedValues(VALUE_S32, eps_).Print();
        }

    } else if (command == "nextint") {
        if(history_.IsEmpty()){
            cout << "No history to search" << endl;
            return;
        }
        int value;
        input_stream >> value;

        search_value sv;
        sv.type = VALUE_S32;
        sv.value.s32 = value;

        history_.GetChangedValues(sv, eps_).Print();
    } else if (command == "writeint" || command == "write") {
        void *address;
        int value;
        input_stream >> address >> value;
        memory_wrapper_.Write<int>(address, value);
    } else if (command == "watch") {
        if(command == input){
            watchlist_.Print();
            return;
        }
        if(input.find("last") != std::string::npos){
            if(history_.IsEmpty()){
                cout << "No Last Search" << std::endl;
                return;
            }
            for (int i = 0; i < history_.last_search.GetSize(); ++i) {
                watchlist_.Add(history_.last_search.addresses.at(i));
            }
            return;
        }
        void* address;
        input_stream >> address;
        if(address){
            watchlist_.Add(address);
            address = NULL;
        }
    } else if (command == "snapshot") {
        unsigned int snapshot_id = snapshot_manager_.SaveSnapshot(memory_wrapper_);
        cout << "Saved snapshot with id " << snapshot_id << std::endl;
    } else if (command == "deletesnapshot"){
        unsigned int snapshot_id;
        input_stream >> snapshot_id;
        snapshot_manager_.DeleteSnapshot(snapshot_id);
        cout << "Removed snapshot with id " << snapshot_id << std::endl;
    } else if (command == "compare") {
        unsigned int old_snapshot_id, new_snapshot_id;
        input_stream >> old_snapshot_id >> new_snapshot_id;

        bool compare_current = input_stream.fail(); // Assume it failed because only the old snapshot id provided
        if (compare_current) {
            new_snapshot_id = snapshot_manager_.SaveSnapshot(memory_wrapper_);
        }

        snapshot_manager_.PrintComparison(old_snapshot_id, new_snapshot_id);

        if (compare_current) {
            snapshot_manager_.DeleteSnapshot(new_snapshot_id);
        }
    } else if (command == "run") {
        tracer_.Run();
    } else if (command == "step") {
        size_t num_steps = 1;
        input_stream >> num_steps;
        tracer_.SingleStep(num_steps);
    } else {
        cout << "Unrecognized command" << std::endl;
    }
}
} // namespace ProcessMemoryViewer