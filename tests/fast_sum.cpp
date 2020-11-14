#include <unistd.h>
#include <iostream>

/* 
This test program sums numbers from 1 to n in a for loop, as fast as possible
Our ProcessMemoryViewer should be able to single step through this program.
*/

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./fast_sum <upper limit>" << std::endl;
        return 1;
    }

    int n = atoi(argv[1]);
    int sum = 0;
    std::cout << "Summing from 1 to " << n << std::endl;

    for (int i = 0; i < n; ++i) {
        sum += i;
    }

    std::cout << "The sum is: " << sum << std::endl;
}