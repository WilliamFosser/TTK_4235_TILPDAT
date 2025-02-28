#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "driver/elevio.h"
#include "queue.h"

typedef MotorDirection Direction; 

typedef struct
{
    int8_t floor; //-1 for undefined 
    Direction direction; 
    
    bool stop_button; 
    bool door_open; 
    bool obstructed; 

    Queue* queue;

    // Needed when the elevator is stopped, and is starting moving again
    uint8_t last_floor; 
    Direction last_direction;
} Elevator;


Elevator* elevator_init(); 
void elevator_state_machine(Elevator *elevator); 


void update_floor(Elevator *elevator);
void open_door(Elevator *elevator);
void close_door(Elevator *elevator); 
void set_direction(Elevator *elevator, int8_t direction);

void check_hall_buttons(Elevator *elevator);
void check_cab_buttons(Elevator *elevator);
void repreoritize_orders(Elevator *elevator);
void move_elevator(Elevator *elevator);

