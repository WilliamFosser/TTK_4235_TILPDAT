#include "elevator.h"

Elevator elevator_init() {
    Elevator elevator = {
        .floor = -1, // Assume undefined floor at initialization
        .direction = DIRN_STOP,
        .stop_button = false,
        .door_open = false,
        .obstructed = false,
        .internal_buttons = {false, false, false, false}
    };

    if (elevio_floorSensor() != -1) {
        elevator.floor = elevio_floorSensor();
    }
    else {
        elevio_motorDirection(DIRN_DOWN);
        while (elevio_floorSensor() == -1) {
            // Wait for elevator to reach a floor
        }
        elevator.floor = elevio_floorSensor();
        elevio_motorDirection(DIRN_STOP);
    }

    return elevator;
};


// Returns the current floor of the elevator. 
// If the elevator is between floors, the function returns -1.
int8_t get_floor() {
    return elevio_floorSensor();
};


void open_door() {
    elevio_doorOpenLamp(1);
};


void close_door() {
    elevio_doorOpenLamp(0);
};


void elevator_state_machine(Elevator *elevator) {
    elevator->floor = get_floor();
    
    switch (elevator->floor)
    {
    case -1:
        /* code */
        break;
    
    case 0:
        /* code */
        break;

    case 1:
        /* code */
        break;

    case 2:
        /* code */
        break;

    case 3:
        /* code */
        break;
    
    default:
        break;
    }
};

