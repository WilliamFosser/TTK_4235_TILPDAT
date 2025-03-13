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

    Direction direction; 
    Direction last_direction;   // Needed when the elevator is stopped, and is starting moving again

    ElevatorState state;

} Elevator;


static Elevator elevator = {
    .floor = -1,                // Assume undefined floor at initialization
    .direction = DIRN_STOP,
    .last_direction = DIRN_STOP,
    .state = STANDBY,
};


void elevator_init() {
    if (elevio_floorSensor() != -1) {
        elevator.floor = elevio_floorSensor();
    }
    else {
        set_direction(DIRN_DOWN);
        while (elevio_floorSensor() == -1) {
            // Wait for elevator to reach a floor
        }
        elevator.floor = elevio_floorSensor();
        set_direction(DIRN_STOP);
        elevio_floorIndicator(elevator.floor);
    }

    printf("Init finished\n");
};


void update_floor() {
    static int last_floor = -1;
    elevator.floor = elevio_floorSensor();
    if (elevator.floor != -1 && last_floor != elevator.floor) {
        elevio_floorIndicator(elevator.floor);
    }
    last_floor = elevator.floor;
}


void open_door() {
    elevator.state = DOOR_OPEN; // Håndtere døråpning i state machine? FJERNE DOOR_OPEN FRA ELEVATOR?
};

// Need rewrite
void close_door() {
    if (!elevio_obstruction()) {
        elevio_doorOpenLamp(0);
        // Set direction to last direction?
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

static Direction opposite_direction(Direction direction) {
    if (direction == DIRN_UP) {
        return DIRN_DOWN;
    }
    else if (direction == DIRN_DOWN) {
        return DIRN_UP;
    }
    else {
        return DIRN_STOP;
    }
}





void elevator_state_machine() {

    printf("Current state: %d\n", elevator.state);
    printf("Current floor: %d\n", elevator.floor);
    printf("Current direction: %d\n", elevator.direction);

    if (queue_empty()) {
        elevator.state = STANDBY;
    }

    if (elevio_stopButton()) {
        elevator.state = STOPPED;
    }


    switch (elevator.state) {
        case STANDBY:
        set_direction(DIRN_STOP);
            if (orders_in_direction(DIRN_UP, elevator.floor)) {
                set_direction(DIRN_UP);
                elevator.state = MOVING;
            }
            else if (orders_in_direction(DIRN_DOWN, elevator.floor)) {
                set_direction(DIRN_DOWN);
                elevator.state = MOVING;
            }
            else {
                set_direction(DIRN_STOP);
            }
            break;
        case MOVING:
            if (!reached_end() && orders_in_direction(elevator.direction, elevator.floor)) {
                if (order_at_floor(elevator.floor) && 
                    !order_in_opposite_direction(elevator.direction, elevator.floor)) {
                    pop_order(elevator.floor);
                    open_door();
                }
            else if (reached_end() || !orders_in_direction(elevator.direction, elevator.floor)) {
                if (order_in_opposite_direction(elevator.direction, elevator.floor)) {
                    set_direction(opposite_direction(elevator.direction));;
                }
                else {
                    elevator.state = STANDBY;
                } 
            }
            }
            break;
        case DOOR_OPEN:
            elevio_doorOpenLamp(1);
            if (elevio_obstruction()) {
                // Loops until obstruction is removed
            }
            else {
                start_timer(3);
                if (timer_expired()) {
                    close_door();
                    elevator.state = MOVING; // sette direction til last direction?
                }
            }
            break;
        case STOPPED:
            set_direction(DIRN_STOP);
            if (elevator.floor != -1) {
                open_door();
            }
            while (elevio_stopButton()) {
                // Wait for stop button to be released
                start_timer(3);
                elevio_stopLamp(1);
            }
            pop_all_orders();
            elevator.state = STANDBY;
            break;
        default:
            break;
        
    }
}