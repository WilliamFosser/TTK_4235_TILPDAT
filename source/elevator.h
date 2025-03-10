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
    uint8_t last_floor; 
    Direction last_direction; // Needed when the elevator is stopped, and is starting moving again
    
    bool stop_button; 
    bool door_open; 
    bool obstructed; 
    bool stop_flag;
    bool isIdle;

    Queue queue;
    
} Elevator;


void elevator_init(Elevator *elevator); 
void elevator_state_machine(Elevator *elevator); 


void update_floor(Elevator *elevator);
void open_door(Elevator *elevator);
void close_door(Elevator *elevator); 
void set_direction(Elevator *elevator, Direction direction);

void check_hall_buttons(Elevator *elevator);
void check_cab_buttons(Elevator *elevator);
void reprioritize_lists(Elevator *elevator);
void check_if_stop(Elevator *elevator);
void move_elevator(Elevator *elevator);



void handle_stop_butn(Elevator *elevator);