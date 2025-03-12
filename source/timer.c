
#include "timer.h"
#include <stdbool.h>
#include <stdio.h>

// Defined in .c file to make the interface minimal. Only one timer is needed.
typedef struct
{
    time_t duration;
    time_t start_time; 
    time_t current_time;
} Timer;


static Timer timer; 


void start_timer(time_t duration) {
    timer.duration = duration;
    time(&timer.start_time);

    printf("Timer started \n");
};


bool timer_expired() {
    time(&timer.current_time);
    time_t time_diff = timer.current_time - timer.start_time;
    if (time_diff >= timer.duration) {
        return true;
        printf("Timer expired after %ld seconds \n", time_diff);
    } else {
        return false;
    }
};