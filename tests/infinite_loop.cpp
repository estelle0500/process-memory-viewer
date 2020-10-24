#include <iostream>
#include <unistd.h>

/*
This test program will run in an infinite loop.
Our ProcessMemoryViewer should be able to overwrite the `status` variable and break out of the loop.
*/
int main() {
    const volatile int status = 0xDEAD;
    std::cout << (void*) &status << std::endl;

    while (status == 0xDEAD) {
        std::cout << "You're stuck here forever!" << std::endl;
        sleep(5);
    }

    std::cout << "Amazing! You found your way out!" << std::endl;
}