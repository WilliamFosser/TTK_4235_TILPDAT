#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "elevio.h"

typedef MotorDirection Direction; 

typedef struct
{
    uint8_t floor; 
    bool on_floor; 
    Direction direction; 
    
    bool stop_button; 
    bool door_open; 
    bool obstructed; 
    bool internal_buttons[4] = {false, false, false, false}; 

} Elevator;


Elevator elevator_init(); 
void elevator_fmt(Elevator& elevator); 
uint8_t get_floor();
bool get_floor_state(); //om vi er på en etasje
void open_door();
void close_door(); 

