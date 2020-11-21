#include <unistd.h>
#include <iostream>

#define SLEEP_TIME 1
#define DELAY 5
#define DEBUG

/* 
This test program increments a non-static integer value to a set iteration
*/

int main() {
    sleep(DELAY);
    int value = 1;
    while(value < 100){
#ifdef DEBUG
        if(value % 10 == 0) std::cout << value << std::endl;
#endif
        value++;
        sleep(SLEEP_TIME);
    }
}