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
    elevator->last_direction = DIRN_STOP;

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
    if (!elevio_obstruction()) {
        elevio_doorOpenLamp(0);
        elevator->door_open = false;
    }
};


// Check if any hall buttons are pressed and update the button lamps and the elevator queue
void check_hall_buttons(Elevator *elevator) {
    bool is_cab_order = false;
    for(int floor = 0; floor < N_FLOORS; floor++){
        for(int button = 0; button < N_BUTTONS-1; button++){ // N_BUTTONS-1 = Exclude cab button
            int buttonPressed = elevio_callButton(floor, button);
            
            if (buttonPressed) {
                add_order(&(elevator->queue), floor, is_cab_order);
                elevio_buttonLamp(floor, button, 1);
            }
        }
    }
}; 


void check_cab_buttons(Elevator *elevator) {
    bool is_cab_order = true;
    for(int floor = 0; floor < N_FLOORS; floor++) {
        int buttonPressed = elevio_callButton(floor, BUTTON_CAB);

        if (buttonPressed) {
            elevio_buttonLamp(floor, BUTTON_CAB, 1);
            add_order(&(elevator->queue), floor, is_cab_order);
        }
    }
};


void set_direction(Elevator *elevator, Direction direction) {
    if (direction != DIRN_STOP) {
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

    if ((!elevator->door_open || !elevator->obstructed) && elevator->isIdle) {
        if (elevator->last_direction == DIRN_UP && elevator->last_floor != 3){
            for (uint8_t floor = elevator->last_floor; floor < N_FLOORS; floor++){
                if(elevator->queue.prioritized_orders[floor]){
                    set_direction(elevator, elevator->last_direction);
                    break;
                }
        }
        }
        else if (elevator->last_direction == DIRN_DOWN && elevator->last_floor != 0){
            for (uint8_t floor = elevator->last_floor; floor < N_FLOORS; floor--){
                if(elevator->queue.prioritized_orders[floor]){
                    set_direction(elevator, elevator->last_direction);
                    break;
                }
            }
        }
        else{
            set_direction(elevator, elevator->last_direction*(-1));
        }
}
};

void check_if_stop(Elevator *elevator){
    int8_t curr_floor = elevator->floor;
    if (curr_floor != -1){
        if(elevator->queue.prioritized_orders[curr_floor]){
            pop_order(&(elevator->queue), curr_floor);
            set_direction(elevator, DIRN_STOP);
            open_door(elevator);
            Timer time;
            start_timer(&time, 3);
            reprioritize_lists(elevator);
            while (!timer_expired(&time)) {
                // Wait for 3 seconds
                check_hall_buttons(elevator);
                check_cab_buttons(elevator);
                handle_stop_butn(elevator);
            }
            close_door(elevator);
        }
    }
}

//FEIL HER ET STED, KAN IKKE FINNE UT HVOR.

void reprioritize_lists(Elevator *elevator){
    bool no_prioritized_orders = true;
    for (int i = 0; i < 4; i++){
        if (elevator->queue.prioritized_orders[i]){
            no_prioritized_orders = false;
            break;
        }
    }
    if (no_prioritized_orders){
        for (int i = 0; i < N_FLOORS; i++){
            elevator->queue.prioritized_orders[i] = elevator->queue.unprioritized_orders[i];
            elevator->queue.unprioritized_orders[i] = false;
            break;
         }
    }
}


void handle_stop_butn(Elevator *elevator){
    Timer timer;
    while (elevio_stopButton()) {
        elevator->stop_flag = true;
        elevio_stopLamp(1);
        set_direction(elevator, DIRN_STOP);

        pop_all_orders(&elevator->queue);

        if (elevator->floor != -1) {
            open_door(elevator);
        }
    }
    printf("2");

    elevio_stopLamp(0);

    if (elevator->door_open && elevator->stop_flag) {
        elevator->stop_flag = false;
        start_timer(&timer, 3);
        
        while (!timer_expired(&timer)) {
            // Wait for 3 seconds
            check_hall_buttons(elevator);
            check_cab_buttons(elevator);
        }
        close_door(elevator);
    }
}