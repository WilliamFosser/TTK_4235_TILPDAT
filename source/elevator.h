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
    bool isIdle;
    int nextFloor;
    bool stopFlag;

    Queue queue; // [4][3] array of orders
    // if ordre i andre etasje, cab: if queue[1][2]==True do th
} Elevator;


void elevator_init(Elevator *elevator); 
void elevator_state_machine(Elevator *elevator); 
bool check_if_stop(Elevator *elevator);
void stop_elevator(Elevator *elevator);

void update_floor(Elevator *elevator);
void open_door(Elevator *elevator);
void close_door(Elevator *elevator); 
void set_direction(Elevator *elevator, Direction direction);

void check_buttons(Elevator *elevator);
Direction move_elevator(Elevator *elevator);



void handle_stop_butn(Elevator *elevator);