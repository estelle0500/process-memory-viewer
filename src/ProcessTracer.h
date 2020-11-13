#pragma once

#include <sys/uio.h>

namespace ProcessMemoryViewer {
/* Manages the child process and sends signal */
class ProcessTracer {
  public:
    /* Starts a child process that will exec the executable */
    void Start(char *executable_name, char **args);

    /* Returns that the process exists */
    bool IsValid() const;

    /* Returns that the wrapped process is still running */
    bool IsRunning() const;

    /* Kill the child process */
    void Kill();

    /* Stops the child process */
    void Pause();

    /* Continues the child process */
    void Continue();

    pid_t pid() const {
        return pid_;
    }

  private:
    pid_t pid_ = -1;
};
} // namespace ProcessMemoryViewer