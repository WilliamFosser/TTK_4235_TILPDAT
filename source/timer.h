#pragma once
#include <time.h>
#include <stdint.h>
#include <stdbool.h>


typedef struct
{
    time_t duration;
    time_t start_time; 
    time_t current_time;
} Timer;

// Starts a timer with a given duration in seconds
void start_timer(Timer *timer, time_t duration);
bool timer_expired(Timer *timer);



