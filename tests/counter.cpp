#include <unistd.h>
#include <iostream>

#define SLEEP_TIME 1
#define DELAY 5
#define DEBUG

/* 
This test program increments a non-static integer value to a set iteration
*/

int main() {

    int value = 1;

    std::cout << "Counter started. Address of value is " << &value << std::endl;
    sleep(DELAY);
    while(value < 100){
#ifdef DEBUG
        if(value % 10 == 0) std::cout << value << std::endl;
#endif
        value++;
        sleep(SLEEP_TIME);
    }
}