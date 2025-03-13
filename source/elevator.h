#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "driver/elevio.h"
#include "queue.h"

typedef MotorDirection Direction; 

typedef enum {
    STANDBY,
    MOVING,
    MOVING_ON_FLOOR,
    DOOR_OPEN,
    OBSTRUCTED,
    STOPPED,
} ElevatorState;


void elevator_init(); 
void elevator_state_machine(); 


void update_floor();
void open_door();
void close_door(); 
void set_direction(Direction direction);
void check_call_buttons();


