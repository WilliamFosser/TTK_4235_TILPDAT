#include "timer.h"
#include <stdbool.h>
#include <stdio.h>

/**
 * @file
 * @brief Starts a timer with a given duration.
 * 
 * @param timer Pointer to the Timer structure.
 * @param duration Duration of the timer in seconds.
 */
void start_timer(Timer *timer, time_t duration) {
    timer->duration = duration;
    time(&timer->start_time);

    printf("Timer started \n");
}

/**
 * @brief Checks if the timer has expired.
 * 
 * @param timer Pointer to the Timer structure.
 * @return true If the timer has expired.
 * @return false If the timer has not expired.
 */
bool timer_expired(Timer *timer) {
    time(&timer->current_time);
    time_t time_diff = timer->current_time - timer->start_time;
    if (time_diff >= timer->duration) {
        printf("Timer expired after %ld seconds \n", time_diff);
        return true;
    } else {
        return false;
    }
}