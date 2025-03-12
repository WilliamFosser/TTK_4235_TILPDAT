#include "elevator.h"
#include "queue.h"
#include "driver/elevio.h"
#include "timer.h"
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>


void elevator_init(Elevator *elevator) {
    elevator->floor = -1;                    // Assume undefined floor at initialization
    elevator->direction = DIRN_STOP;
    elevator->stop_button = false;
    elevator->door_open = false;
    elevator->obstructed = false;
    elevator->stop_flag = false; 
    elevator->isIdle = false;
    elevator->nextFloor = -1;

    new_queue(&(elevator->queue));
    pop_all_orders(&(elevator->queue));

    if (elevio_floorSensor() != -1) {
        elevator->floor = elevio_floorSensor();
    }
    else {
        set_direction(elevator, DIRN_DOWN);
        while (elevio_floorSensor() == -1) {
            // Wait for elevator to reach a floor
        }
        elevator->floor = elevio_floorSensor();
        elevator->last_floor = elevator->floor;
        set_direction(elevator, DIRN_STOP);
    }
    elevator->isIdle = true;
    elevator->last_direction = DIRN_DOWN;
    printf("Init finished\n");
};


void update_floor(Elevator *elevator) {
    elevator->floor = elevio_floorSensor();
    if (elevator->floor != -1) {
        elevator->last_floor = elevator->floor;
        elevio_floorIndicator(elevator->floor);
    }
};


void open_door(Elevator *elevator) {
    elevio_doorOpenLamp(1);
    elevator->door_open = true;
};


void close_door(Elevator *elevator) {
    while(elevio_obstruction()){
        Timer time;
        start_timer(&time, 1);
        while (!timer_expired(&time)) {
                printf("Obstructed\n");
                check_buttons(elevator);
                handle_stop_butn(elevator);
        }
    }
    if (!elevio_obstruction()) {
        elevio_doorOpenLamp(0);
        elevator->door_open = false;
        elevator->isIdle = true;
    }
};


// Check if any hall buttons are pressed and update the button lamps and the elevator queue
void check_buttons(Elevator *elevator) {
    for(int floor = 0; floor < N_FLOORS; floor++){
        for(int button = 0; button < N_BUTTONS; button++){ 
            int buttonPressed = elevio_callButton(floor, button);
            if (buttonPressed) {
                add_order(&(elevator->queue), floor, button);
                elevio_buttonLamp(floor, button, 1);
            }
        }
    }
}; 




void set_direction(Elevator *elevator, Direction direction) {
    if (direction != DIRN_STOP) {
        elevator->isIdle = false;
    }
    if (elevator->direction != DIRN_STOP) {
        elevator->last_direction = elevator->direction;
    }
    elevator->direction = direction;
    elevio_motorDirection(elevator->direction);
    if(direction == DIRN_STOP){elevator->isIdle = true;}
};



//==============================================================================
//============================== Direction Algorithm ===========================
//==============================================================================


void move_elevator(Elevator *elevator) {
    //TODO: Implement move_elevator function
    // This function should move the elevator to the next floor in the queue
    int8_t last_dirr = elevator->last_direction;
    if ((!elevator->door_open || !elevator->obstructed) && elevator->isIdle) {
        if (elevator->floor == -1){
            elevator_init(elevator);
        }
        printf("Moving elevator. Last direction was %d\n. Is idle: %d\n", last_dirr, elevator->isIdle);
        uint8_t curr_floor = elevator->floor;
        Direction setDir = DIRN_STOP;
        bool hasChanged = false;
        if (curr_floor == 0 || elevator->last_direction == DIRN_UP) {
            for (int8_t i = curr_floor; i < N_FLOORS; i++){
                if (elevator->queue.orders[i] != &(elevator->queue.order_type[3])){
                    if(*(elevator->queue.orders[i]) == 0 || *(elevator->queue.orders[i]) == 1){
                        elevator->nextFloor = i;
                        setDir = DIRN_UP;
                        hasChanged = true;
                    }
                }
            }
        }
        if(curr_floor == 3 || last_dirr == DIRN_DOWN) {
            for (int8_t i = curr_floor; i >= 0; i--){
                if (elevator->queue.orders[i] != &(elevator->queue.order_type[3])){
                    if(*(elevator->queue.orders[i]) == 0 || *(elevator->queue.orders[i]) == -1){
                        elevator->nextFloor = i;
                        setDir = DIRN_DOWN;
                        hasChanged = true;
                    }
                }
            }
        }
        if(!hasChanged) {
            for (int i = 0; i < 4; i++){
                if (elevator->queue.orders[i] != &(elevator->queue.order_type[3])){
                    if (i > curr_floor){
                        elevator->last_direction = DIRN_UP;
                        elevator->nextFloor = i;
                        setDir = DIRN_UP;
                        printf("Going up\n");
                    }   
                    else if (i < curr_floor){
                        elevator->last_direction = DIRN_DOWN;
                        elevator->nextFloor = i;
                        setDir = DIRN_DOWN;
                        printf("Going down\n");
                    }
                    break;
                }
            }
        }
        set_direction(elevator, setDir);
    while (elevator->floor == curr_floor && elevator->direction != DIRN_STOP) {
        update_floor(elevator);
        check_if_stop(elevator);
        check_buttons(elevator);
        handle_stop_butn(elevator);
        }
    };
}
void check_if_stop(Elevator *elevator){
    int8_t curr_floor = elevator->floor;
    if (curr_floor != -1){
        if(elevator->queue.orders[curr_floor] != &(elevator->queue.order_type[3])){
            if(*(elevator->queue.orders[curr_floor]) == 0 || *(elevator->queue.orders[curr_floor]) == elevator->direction || elevator->nextFloor == curr_floor){
                pop_order(&(elevator->queue), curr_floor);
                set_direction(elevator, DIRN_STOP);
                open_door(elevator);
                Timer time;
                start_timer(&time, 3);
                while (!timer_expired(&time)) {
                    // Wait for 3 seconds
                    check_buttons(elevator);
                    handle_stop_butn(elevator);
                }
                close_door(elevator);
            }
        }
    }
};



void handle_stop_butn(Elevator *elevator){
    Timer timer;
    elevator->stop_flag = false;
    if (elevio_stopButton()) {
        elevator->stop_flag = true;
        Timer delay;
        start_timer(&delay, 1);
        while (!timer_expired(&delay));
    }
    while (elevator->stop_flag) {
        elevio_stopLamp(1);
        set_direction(elevator, DIRN_STOP);

        pop_all_orders(&elevator->queue);

        if (elevator->floor != -1) {
            open_door(elevator);
        }
        if(elevio_stopButton()){
            elevator->stop_flag = false;
        }
    }
    printf("2");

    elevio_stopLamp(0);

    if (elevator->door_open && elevator->stop_flag) {
        elevator->stop_flag = false;
        start_timer(&timer, 3);
        
        while (!timer_expired(&timer)) {
            // Wait for 3 seconds
            check_buttons(elevator);
            handle_stop_butn(elevator);
        }
        close_door(elevator);
    }
}