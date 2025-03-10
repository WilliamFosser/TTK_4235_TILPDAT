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
    if (!elevio_obstruction()) {
        elevio_doorOpenLamp(0);
        elevator->door_open = false;
        elevator->isIdle = true;
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
        printf("Moving elevator. Last direction was %d\n. Is idle: %d\n", last_dirr, elevator->isIdle);
        uint8_t curr_floor = elevator->floor;
        if (curr_floor == 0 || elevator->last_direction == DIRN_UP) {
            for (int8_t i = curr_floor; i < N_FLOORS; i++){
                if (elevator->queue.prioritized_orders[i]){
                    printf("Setting direction to up\n");
                    set_direction(elevator, DIRN_UP);
                    break;
                }
            }
        }
        if (curr_floor == 3 || last_dirr == DIRN_DOWN) {
            for (int8_t i = curr_floor; i >= 0; i--){
                if (elevator->queue.prioritized_orders[i]){
                    set_direction(elevator, DIRN_DOWN);
                    printf("Setting direction to down\n");
                    break;
                }
            }
        }
        if (elevator->direction == DIRN_STOP){
            for (int i = 0; i < 4; i++){
                if (elevator->queue.prioritized_orders[i]){
                    if (i > curr_floor){
                        set_direction(elevator, DIRN_UP);
                        printf("Setting direction to up\n");
                    }
                    else if (i < curr_floor){
                        set_direction(elevator, DIRN_DOWN);
                        printf("Setting direction to down\n");
                    }
                    break;
                }
            }
        }
    while (elevator->floor == curr_floor && elevator->direction != DIRN_STOP) {
        update_floor(elevator);
        check_if_stop(elevator);
        check_hall_buttons(elevator);
        check_cab_buttons(elevator);
        handle_stop_butn(elevator);
        }
    };
}
void check_if_stop(Elevator *elevator){
    int8_t curr_floor = elevator->floor;
    if (curr_floor != -1){
        if(elevator->queue.prioritized_orders[curr_floor] || elevator->queue.unprioritized_orders[curr_floor]){
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


void reprioritize_lists(Elevator *elevator){
    bool no_prioritized_orders = true;
    int copy_unprio[4] = {0};
    for (int i = 0; i < 4; i++){
        if (elevator->queue.prioritized_orders[i]){
            no_prioritized_orders = false;
        }
    }
    if (no_prioritized_orders){
        for (int i = 0; i < N_FLOORS; i++){
            if(copy_unprio[i]){
                pop_order(&(elevator->queue), i);
                add_order(&(elevator->queue), i, true);
            }
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