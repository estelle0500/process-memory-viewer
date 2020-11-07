#include <iostream>
#include <unistd.h>
/*
This test program will run in an infinite loop.
Our ProcessMemoryViewer should be able to overwrite the `status` variable and break out of the loop.
*/
int main() {
    static int l    = 1336;
    float f1        = 1337.0;
    float f2        = 1337.5;
    int leet        = 1337;
    double l33t     = 1337.78;

    double d1     = 1400.25;
    double d2     = 1400.50;
    double d3     = 1400.75;
    double d4     = 1400.39;

    std::cout << "Types started: find 1337!" << std::endl;
    std::cout << f1 << "\t\t" << &f1 << "\n";
    std::cout << f2 << "\t\t" << &f2 << "\n";
    std::cout << leet << "\t\t" << &leet << "\n";
    std::cout << l33t << "\t\t" << &l33t << "\n";

    while(1){
        sleep(10);
    }
}