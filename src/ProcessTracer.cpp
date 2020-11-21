#include "ProcessTracer.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/personality.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <sstream>

namespace ProcessMemoryViewer {
void ProcessTracer::Start(char *executable_name, char **args) {
    int fork_code = fork();
    if (fork_code == 0) { // Child
        ptrace(PTRACE_TRACEME, 0, NULL, 0);
        personality(ADDR_NO_RANDOMIZE);
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

    // Replace instruction at `main` with a trapping instruction
    void *main_address = FindMainAddress();
    std::cout << main_address << std::endl;
    long orig = ptrace(PTRACE_PEEKTEXT, pid_, main_address, NULL);
    long trap = (orig & ~0xff) | 0xcc;
    ptrace(PTRACE_POKETEXT, pid_, main_address, trap);

    // Continue program until just before main, then restore original main instruction
    Continue();
    if (waitpid(pid_, &status, 0) == -1) {
        perror("Couldn't wait on child process");
        exit(1);
    }

    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid_, NULL, &regs);
    regs.rip = (unsigned long long) main_address;
    ptrace(PTRACE_SETREGS, pid_, NULL, &regs);
    ptrace(PTRACE_POKETEXT, pid_, main_address, orig);
}

void *ProcessTracer::FindMainAddress() {
    char *command = NULL;
    asprintf(&command, "objdump -d /proc/%d/exe | grep \\<main\\>:", pid_);

    FILE *command_output = popen(command, "r");
    if (command_output == NULL) {
        perror("popen failed");
        return NULL;
    }

    const size_t offset = 0x555555554000;
    char *address = NULL;
    fscanf(command_output, "%p", &address);
    return address + offset;
}

void ProcessTracer::Run() {
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid_, NULL, &regs);
    printf("Program counter is 0x%llx\n", regs.rip);

    int status;
    Continue();
    waitpid(pid_, &status, 0);
}

void ProcessTracer::SingleStep(size_t num_steps) {
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid_, NULL, &regs);
    printf("Program counter is 0x%llx\n", regs.rip);
    
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