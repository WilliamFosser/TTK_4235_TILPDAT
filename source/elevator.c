#include "elevator.h"
#include "queue.h"
#include "driver/elevio.h"
#include "timer.h"
#include <stdbool.h>
#include <stdio.h>
#include <time.h>


typedef struct
{
    int8_t floor;               // -1 for undefined 
    uint8_t last_floor; 

    Direction direction; 
    Direction last_direction;   // Needed when the elevator is stopped, and is starting moving again

    ElevatorState state;
    
    bool stop_button; 
    bool door_open; 
    bool obstructed; 
    bool stop_flag;    
} Elevator;

static Elevator elevator = {
    .floor = -1,                // Assume undefined floor at initialization
    .last_floor = 0,
    .direction = DIRN_STOP,
    .last_direction = DIRN_UP,
    .state = STANDBY,

    .stop_button = false,
    .door_open = false,
    .obstructed = false,
    .stop_flag = false
};


void elevator_init() {
    //pop_all_orders();        // Sannsynligvis ikke nødvendig

    if (elevio_floorSensor() != -1) {
        elevator.floor = elevio_floorSensor();
        elevator.last_floor = elevator.floor;
    }
    else {
        set_direction(DIRN_DOWN);
        while (elevio_floorSensor() == -1) {
            // Wait for elevator to reach a floor
        }
        elevator.floor = elevio_floorSensor();
        elevator.last_floor = elevator.floor;
        set_direction(DIRN_STOP);
    }
    printf("Init finished\n");
};


void update_floor() {
    static last_floor = -1;
    elevator.floor = elevio_floorSensor(); {
    if (elevator.floor != -1 && last_floor != elevator.floor) 
        elevio_floorIndicator(elevator.floor);
    }
    last_floor = elevator.floor;
};


void open_door() {
    elevio_doorOpenLamp(1);
    elevator.door_open = true;
};

// Need rewrite
void close_door() {
    if (!elevio_obstruction()) {
        elevio_doorOpenLamp(0);
        elevator.door_open = false;
        if (!(elevator.floor == 0 && elevator.last_direction == DIRN_DOWN) && 
        !(elevator.floor == 3 && elevator.last_direction == DIRN_UP)){
            //set_direction(elevator, elevator->last_direction); //Avoids going outside range after stop
        }
    }
};

void check_call_buttons() {
    for(int floor = 0; floor < N_FLOORS; floor++) {
        for(int button = 0; button < N_BUTTONS; button++) {
            int buttonPressed = elevio_callButton(floor, button);
            
            if (buttonPressed) {
                add_order(floor, button);
                elevio_buttonLamp(floor, button, 1);
            }
        }
    }
}




void set_direction(Direction direction) {
    if (elevator.direction != DIRN_STOP) {
        elevator.last_direction = elevator.direction;
    }
    printf("Last direction: %d \n", elevator.last_direction);
    elevator.direction = direction;
    elevio_motorDirection(direction);
};

bool reached_end() {
    if (elevator.floor == 0 && elevator.direction == DIRN_DOWN) {
        return true;
    }
    if (elevator.floor == N_FLOORS-1 && elevator.direction == DIRN_UP) {
        return true;
    }
    return false;
};

bool hall



//void reprioritize_orders(Elevator *elevator);



// Reimplement 
void move_elevator() {
    printf("Last floor: %d \n",elevator.last_floor);
    bool no_order_flag_1 = false;
    bool no_order_flag_2 = false;

    if (elevator.last_direction == DIRN_DOWN || elevator.last_floor == N_FLOORS-1) {
        printf("Entered moving down section\n");
        for (uint8_t order = elevator.last_floor; order > 0; order--) {
            if (elevator.queue.cab_orders[order]) {
                set_direction(DIRN_DOWN);
                break;
            }
            if (order == N_FLOORS-1) {no_order_flag_1 = true;}
        }
    }
    if (elevator.last_direction == DIRN_UP || elevator.last_floor == 0) {
        printf("Entered moving up section\n");
        for (uint8_t order = elevator->last_floor; order < N_FLOORS; order++) {
            if (elevator->queue.cab_orders[order]) {
                set_direction(elevator,DIRN_UP);
                break;
            }
            if (order == 0) {no_order_flag_2 = true;}
        }
    }

    
    if (no_order_flag_1 && no_order_flag_2) {
        set_direction(DIRN_STOP);
        elevator.stop_flag = true;
        open_door(elevator);
    }


    if (elevator->queue.cab_orders[elevator.last_floor]) {
        printf("Fant etasje\n");
        set_direction(DIRN_STOP);
        elevator.stop_flag = true;
        open_door(elevator);
        pop_order(elevator.last_floor);
    }
};

