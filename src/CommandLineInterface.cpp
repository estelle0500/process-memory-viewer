#include "CommandLineInterface.h"
#include "memory/MemoryHistory.h"

#include <dirent.h>         // For DIR
#include <fstream>

namespace ProcessMemoryViewer {
static constexpr char PROC_DIRECTORY[] = "/proc/";
using std::cout;
using std::endl;
using std::string;

pid_t get_proc_pid(std::string procName) {
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

        string line = get_proc_name(id);

        if (procName == line) {
            pid = id;
            break;
        }
    }

    closedir(proc_dir);
    return pid;
}

string get_proc_name(pid_t pid){
    string cmdPath = string(PROC_DIRECTORY) + std::to_string(pid) + "/cmdline";
    std::ifstream cmdFile(cmdPath.c_str());
    string name;
    getline(cmdFile, name);

    size_t pos = name.find('\0');
    if (pos != string::npos) {
        name = name.substr(0, pos);
    }
    return name;
}

vector<string> CommandLineInterface::split(const string str) {
    const char delimiter = ' ';
    vector<string> tokens;
    std::stringstream ss(str);
    string token;

    while(getline(ss, token, delimiter)){
        tokens.push_back(token);
    }

    return tokens;
}

void CommandLineInterface::HandleInput(std::string input) {

    std::istringstream input_stream(input);
    string command;

    vector<string> argv = split(input);

    input_stream >> command;

    if (!tracer_.IsRunning() && argv[0] != "attach") {
        printf("Child process has stopped. Use 'attach' command.\n");
        return;
    }
    memory_wrapper_.ParseMaps();
    MemorySnapshot current(memory_wrapper_);

    if (command == "info") {
        cout << "Target Pid:\t\t\t" << tracer_.pid() << endl;
        cout << "Target name:\t\t\t" << get_proc_name(tracer_.pid()) << endl;
    } else if (command == "attach") {
        uint target;
        input_stream >> target;
        if(tracer_.ChangeTarget((pid_t)target)){
            memory_wrapper_.SetPid((pid_t)target);
            cout << "Attached to Process" << target << endl;
        } else {
            cout << "Invalid Process: " << target << endl;
        }

    } else if (command == "cont") {
        tracer_.Continue();
    } else if (command == "getpid") {
        std::string name;
        input_stream >> name;
        cout << get_proc_pid(name) << std::endl;
    } else if (command == "getname") {
        int pid;
        input_stream >> pid;
        cout << get_proc_name(pid) << endl;
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
        if(argv[1] == "!"){
            address = (void*)history_.last_modified_address;
        } else {
            history_.last_modified_address = (long) address;
        }

        cout << memory_wrapper_.Read<int>(address) << std::endl;
    } else if (command == "find") {
        if (argv.size() < 2) {
            cout << "Usage: find <value> [options]" << endl;
            return;
        }

        bool narrow = input.find("-n") != std::string::npos && !history_.IsEmpty();
        bool print = input.find("-p") != std::string::npos;

        char *p;
        long converted = std::strtod(argv[1].c_str(), &p);
        if (*p && !narrow) {
            cout << "Usage: find <value> [options]" << endl;
            return;
        }
        bool value_provided = !*p;

        MemoryList *find_results;
        for (int i = 1; i < argv.size(); ++i) {
            string opt = argv[i];
            if (opt == "-l") {
                long value = std::strtol(argv[1].c_str(), &p, 10);
                if (narrow) {
                    MemoryList ml;
                    if (value_provided) {
                        search_value s;
                        s.value.f64 = value;
                        ml = history_.last_search->GetChangedValues(s, eps_);
                    } else {
                        ml = history_.last_search->GetChangedValues(VALUE_F64, eps_);
                    }
                    if (!ml.IsEmpty()) {
                        last_results = ml;
                    }
                    break;
                }
                last_results = current.SearchValue<long>(value, eps_, VALUE_S64, memory_wrapper_);
                find_results = &last_results;
                break;
            } else if (opt == "-f") {
                float value = std::strtof(argv[1].c_str(), &p);
                    if(narrow) {
                        MemoryList ml;
                        if (value_provided) {
                            search_value s;
                            s.value.f64 = value;
                            ml = history_.last_search->GetChangedValues(s, eps_);
                        } else {
                            ml = history_.last_search->GetChangedValues(VALUE_F64, eps_);
                        }
                        if (!ml.IsEmpty()) {
                            last_results = ml;
                        }
                        break;
                    }

                last_results = current.SearchValue<float>(value, eps_, VALUE_F32, memory_wrapper_);
                find_results = &last_results;
                break;
            } else if (opt == "-d") {
                double value = std::strtod(argv[1].c_str(), &p);
                if(narrow){
                    MemoryList ml;
                    if(value_provided){
                        search_value s;
                        s.value.f64 = value;
                        ml = history_.last_search->GetChangedValues(s, eps_);
                    } else {
                        ml = history_.last_search->GetChangedValues(VALUE_F64, eps_);
                    }
                    if(!ml.IsEmpty()){
                        last_results = ml;
                    }
                    break;
                }

                last_results = current.SearchValue<double>(value, eps_, VALUE_F64, memory_wrapper_);
                find_results = &last_results;
                break;
            }
            if(i == argv.size()-1){
                int value = std::strtol(argv[1].c_str(), &p, 10);
                if(narrow){
                    MemoryList ml;
                    if(value_provided){
                        search_value s;
                        s.value.s32 = value;
                        ml = history_.last_search->GetChangedValues(s, eps_);
                    } else {
                        ml = history_.last_search->GetChangedValues(VALUE_S32, eps_);
                    }
                    if(!ml.IsEmpty()){
                        last_results = ml;
                    }
                    break;
                }
                last_results = current.SearchValue<int>(value, eps_, VALUE_S32, memory_wrapper_);
                find_results = &last_results;
                break;
            }
        }
        find_results = &last_results;
        find_results->PrintCount();
        if (!find_results->IsEmpty()) {
            history_.last_search = find_results;
            if(print){
                history_.last_search->Print();
            }
        }
    } else if (command == "display"){
        history_.last_search->Print();
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
            //history_.GetChangedValues(VALUE_S32, eps_).Print();
        }
    } else if (command == "write") {
        if(argv.size() < 3){
            cout << "Usage: write <address> <value>" << endl;
            return;
        }
        char *p;
        void *address;
        int value;
        if(argv[1] == "!"){
            address = (void*)history_.last_modified_address;
        } else {
            input_stream >> address;
            history_.last_modified_address = (long)address;
        }

        value = std::strtol(argv[2].c_str(), &p, 10);

        memory_wrapper_.Write<int>(address, value);
    } else if (command == "set") {
        long value;
        input_stream >> value;
        for (int i = 0; i < history_.last_search->GetSize(); ++i) {
            void* addr = history_.last_search->addresses[i];
            memory_wrapper_.Write<long>(addr, value);
        }
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
            for (int i = 0; i < history_.last_search->GetSize(); ++i) {
                watchlist_.Add(history_.last_search->addresses.at(i));
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