#pragma once
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @file
 * @brief Structure representing a timer.
 */
typedef struct
{
    time_t duration; ///< Duration of the timer in seconds.
    time_t start_time; ///< Start time of the timer.
    time_t current_time; ///< Current time.
} Timer;

/**
 * @brief Starts a timer with a given duration in seconds.
 * 
 * @param timer Pointer to the Timer structure.
 * @param duration Duration of the timer in seconds.
 */
void start_timer(Timer *timer, time_t duration);

/**
 * @brief Checks if the timer has expired. Need to be polled constantly in a loop. 
 * If main loop contains time consuming operations, the timer will not be accurate.
 * 
 * @param timer Pointer to the Timer structure.
 * @return true If the timer has expired.
 * @return false If the timer has not expired.
 */
bool timer_expired(Timer *timer);



