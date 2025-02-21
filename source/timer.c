
#include "timer.h"
#include <stdbool.h>
#include <stdio.h>



Timer start_timer(time_t duration) {
    Timer timer;
    timer.duration = duration;
    time(&timer.start_time);

    printf("Timer started \n");

    return timer;
};


bool timer_expired(Timer *timer) {
    time(&timer->current_time);
    time_t time_diff = timer->current_time - timer->start_time;
    if (time_diff >= timer->duration) {
        return true;
        printf("Timer expired after %ld seconds \n", time_diff);
    } else {
        return false;
    }
};