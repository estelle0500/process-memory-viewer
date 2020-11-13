#include "ProcessTracer.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>

namespace ProcessMemoryViewer {
void ProcessTracer::Start(char *executable_name, char **args) {
    int fork_code = fork();
    if (fork_code == 0) { // Child
        execvp(executable_name, args);
        exit(1);
    }

    pid_ = fork_code;
    std::cout << "Process started with PID: " << pid_ << std::endl;
}

bool ProcessTracer::IsValid() const {
    return pid_ != -1;
}

bool ProcessTracer::IsRunning() const {
    if (!IsValid()) {
        return false;
    }
    
    struct stat status;
    if (stat((("/proc/" + pid_)), &status) != -1) {
        return false;
    }
}

void ProcessTracer::Kill() {
    kill(pid_, SIGTERM);
    int child_status;
    waitpid(pid_, &child_status, 0);
}

void ProcessTracer::Pause() {
    kill(pid_, SIGSTOP);
}

void ProcessTracer::Continue() {
    kill(pid_, SIGCONT);
}
} // namespace ProcessMemoryViewer