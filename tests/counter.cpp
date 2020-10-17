#include <unistd.h>
#include <iostream>

#define SLEEP_TIME 1000

/* 
This test program increments a non-static integer value infinitely
*/

int main() {
    int value = 0;
    while(1){
        value++;
        std::cout << value << std::endl;
        sleep(SLEEP_TIME);
    }
}