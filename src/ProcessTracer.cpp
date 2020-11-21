#include "ProcessTracer.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <sstream>

namespace ProcessMemoryViewer {
void ProcessTracer::Start(char *executable_name, char **args) {
    int fork_code = fork();
    if (fork_code == 0) { // Child
        std::cout << "Trace me" << std::endl;
        ptrace(PTRACE_TRACEME, 0, NULL, 0);
        execvp(executable_name, args);
        exit(1);
    }

    pid_ = fork_code;
    int status;
    if (waitpid(pid_, &status, 0) == -1) {
        perror("Couldn't wait on child process");
        exit(1);
    }
    std::cout << "Process started with PID: " << pid_ << std::endl;
}

void ProcessTracer::Run() {
    int status = 1407;
    int in_call = 0;
    struct user_regs_struct regs;

    while (status == 1407) {
        ptrace(PTRACE_GETREGS, pid_, NULL, &regs) == -1;
        if (!in_call) {
            printf("SystemCall %llu called with %llu, %llu, %llu\n",regs.orig_rax, regs.rbx, regs.rcx, regs.rdx);
        }
        in_call = 1 - in_call; 
        
        if (ptrace(PTRACE_SYSCALL, pid_, NULL, NULL) == -1) {
            break;
        }
        if (waitpid(pid_, &status, 0) == -1) {
            break;
        }
    }
}

void ProcessTracer::SingleStep(size_t num_steps) {
    for (size_t i = 0; i < num_steps; ++i) {
        if (ptrace(PTRACE_SINGLESTEP, pid_, NULL, 0) == -1) {
            perror("Ptrace single step failed");
        }

        int status;
        waitpid(pid_, &status, 0);
    }
}

bool ProcessTracer::IsValid() const {
    return pid_ != -1;
}

bool ProcessTracer::IsRunning() const {
    if (!IsValid()) {
        return false;
    }
    
    struct stat status;
    std::ostringstream path;
    path << "/proc/" << pid_;
    return stat(path.str().c_str(), &status) != -1;
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
    ptrace(PTRACE_CONT, pid_, 0, 0);
}
} // namespace ProcessMemoryViewer