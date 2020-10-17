#include <unistd.h>
#include <iostream>

#define SLEEP_TIME 1
#define DEBUG

/* 
This test program increments a non-static integer value infinitely
*/

int main() {
    int value = 0;
    while(value < 100){
#ifdef DEBUG
        if(value % 10 == 0) std::cout << value << std::endl;
#endif
        value++;
        sleep(SLEEP_TIME);
    }
}