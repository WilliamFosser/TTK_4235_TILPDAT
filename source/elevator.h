#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "driver/elevio.h"

typedef MotorDirection Direction; 

typedef struct
{
    int8_t floor; //-1 for undefined 
    Direction direction; 
    
    bool stop_button; 
    bool door_open; 
    bool obstructed; 
    bool internal_buttons[4];

} Elevator;


Elevator elevator_init(); 
void elevator_state_machine(Elevator *elevator); 


int8_t get_floor();
void open_door();
void close_door(); 

