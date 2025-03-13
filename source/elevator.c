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
    elevator->isIdle = false;
    elevator->nextFloor = -1;
    elevator->stopFlag = false;
    elevio_doorOpenLamp(0);

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
    elevio_floorIndicator(elevio_floorSensor());
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
        check_buttons(elevator);
        handle_stop_butn(elevator);
    }
    if (!elevio_obstruction()) {
        Timer timer;
        start_timer(&timer, 3);
        while (!timer_expired(&timer)) {
            // Wait for 3 seconds
            check_buttons(elevator);
            handle_stop_butn(elevator);
        }
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




void set_direction(Elevator *elevator, Direction direction) { //Sets motor direction and updates bool isIdle
    if (direction != DIRN_STOP) {
        elevator->isIdle = false;
    }
    else{
        elevator->isIdle = true;
    }
    if (elevator->direction != DIRN_STOP) {
        elevator->last_direction = elevator->direction;
    }
    elevator->direction = direction;
    elevio_motorDirection(elevator->direction);
};



//==============================================================================
//============================== Direction Algorithm ===========================
//==============================================================================


Direction move_elevator(Elevator *elevator) {
    //TODO: Implement move_elevator function
    // This function should move the elevator to the next floor in the queue
    if (elevator->direction != DIRN_STOP) {
        return elevator->direction;
    }
    if (elevator->stopFlag) {
        return DIRN_STOP;
    }
    int8_t last_dirr = elevator->last_direction;
    switch (last_dirr){
        case DIRN_UP:
            for (int i = elevator->floor; i < 4; i++){
                    if (elevator->queue.orders[i][BUTTON_HALL_UP] || elevator->queue.orders[i][BUTTON_CAB]){
                        return DIRN_UP;
                    }
                }
            for (int i = 0; i < 4;i++){
                for (int j = 0; j < 3; j++){
                    if (elevator->queue.orders[i][j]){
                        if (i < elevator->floor){
                            return DIRN_DOWN;
                        }
                        else{
                            return DIRN_UP;
                        }
                    }
                }
            }
            return DIRN_STOP;
            break;
        case DIRN_DOWN:
            for (int i = elevator->floor; i >= 0; i--){
                    if (elevator->queue.orders[i][BUTTON_HALL_DOWN] || elevator->queue.orders[i][BUTTON_CAB]){
                        return DIRN_DOWN;
                    }
                }
            for (int i = 0; i < 4; i++){
                for (int j = 0; j < 3; j++){
                    if (elevator->queue.orders[i][j]){
                        if (i < elevator->floor){
                            return DIRN_DOWN;
                        }
                        else{
                            return DIRN_UP;
                        }
                    }
                }
            }
            return DIRN_STOP;
            break;
        default:
                return DIRN_STOP;
                break;
    }
}

bool check_if_stop(Elevator *elevator) {
    if (elevator->floor == -1) {
        return false;
    }
    if (elevator->direction == DIRN_UP && elevator->floor == 3) { //Prevent out of bounds top
        return true;
    }
    if (elevator->direction == DIRN_DOWN && elevator->floor == 0) { //Prevent out of bounds bottom
        return true;
    }
    int8_t lastOrder = 0;
    switch (elevator->direction) {
    case (DIRN_UP):
        if (elevator->queue.orders[elevator->floor][BUTTON_HALL_UP] || elevator->queue.orders[elevator->floor][BUTTON_CAB]) {
            return true;
    }
    case (DIRN_DOWN):
        if (elevator->queue.orders[elevator->floor][BUTTON_HALL_DOWN] || elevator->queue.orders[elevator->floor][BUTTON_CAB]) {
            return true;
        }
    default:
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 3; j++) {
                if (elevator->queue.orders[i][j]) {
                    lastOrder++;
                }
            }
        }
        if (lastOrder == 1) {
            for (int8_t btn = 0; btn < 3; btn++) {
                if (elevator->queue.orders[elevator->floor][btn]) {
                    return true;
                }
            }
        }
    return false;
    }
}

void stop_elevator(Elevator *elevator){
    pop_order(&(elevator->queue), elevator->floor);
    set_direction(elevator, DIRN_STOP);
    open_door(elevator);
    close_door(elevator);
}

void handle_stop_butn(Elevator *elevator){
    if (elevio_stopButton()){
        elevator->stopFlag = true;
        pop_all_orders(&(elevator->queue));
        set_direction(elevator, DIRN_STOP);
        elevio_stopLamp(1); 
        if (elevio_floorSensor() != -1){
            open_door(elevator);
        }
    }
    else{
        return;
    }
    while (elevio_stopButton()){
        printf("Stop button down\n");
    }
    elevator->stopFlag = false;
    elevio_stopLamp(0);
    if (elevio_floorSensor()==1){
        set_direction(elevator, DIRN_DOWN);
    }
    if (elevator->door_open) {
        close_door(elevator);
    }
}