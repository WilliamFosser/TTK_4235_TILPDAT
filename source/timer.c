
#include "timer.h"
#include <stdbool.h>
#include <stdio.h>



void start_timer(time_t duration) {
    Timer timer;
    timer.duration = duration;
    time(&timer.start_time);
    
    bool timer_active = true;
    printf("Timer started \n");

    while (timer_active) {
        time(&timer.current_time);
        time_t time_diff = timer.current_time - timer.start_time;
        if (time_diff >= timer.duration) {
            printf("%ld seconds have passed \n", time_diff);
            timer_active = false;
            break; 
        }
    }
};





