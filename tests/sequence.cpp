#include <unistd.h>
#include <iostream>

/* 
This test program fills up an array with numbers 0, 1, 2, ...
Our ProcessMemoryViewer should be able to read these values.
*/

constexpr size_t NUM_ELEMENTS = 1000;

int main() {
    int array[NUM_ELEMENTS];
    std::cout << "Array starts at " << array << std::endl;

    for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
        array[i] = i;
    }

    sleep(100);
}