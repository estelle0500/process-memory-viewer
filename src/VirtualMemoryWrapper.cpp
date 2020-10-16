#include "VirtualMemoryWrapper.h"
#include <cassert>
#include <sys/uio.h>

namespace ProcessMemoryViewer {
VirtualMemoryWrapper::VirtualMemoryWrapper(pid_t process_id) : process_id_(process_id) {}

char VirtualMemoryWrapper::ReadByte(void *address) {
    char byte_read;
    struct iovec remote_iov{address, 1};
    struct iovec local_iov{&byte_read, 1};
    ssize_t num_bytes_read = process_vm_readv(process_id_, &local_iov, 1, &remote_iov, 1, 0);

    assert(num_bytes_read == 1);
    return byte_read;
}

int VirtualMemoryWrapper::ReadInt(void *address) {
    int int_read;
    struct iovec remote_iov{address, sizeof(int)};
    struct iovec local_iov{&int_read, sizeof(int)};
    ssize_t num_bytes_read = process_vm_readv(process_id_, &local_iov, 1, &remote_iov, 1, 0);

    assert(num_bytes_read == sizeof(int));
    return int_read;
}
} // namespace ProcessMemoryViewer