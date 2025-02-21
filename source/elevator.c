#include "elevator.h"

Elevator elevator_init() {
    Elevator elevator = {
        .floor = -1, // Assume undefined floor at initialization
        .direction = DIRN_STOP,
        .stop_button = false,
        .door_open = false,
        .obstructed = false,

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

// Check if any hall buttons are pressed and update the button lamps and the elevator queue
void check_hall_buttons() {
    for(int floor = 0; floor < N_FLOORS; floor++){
        for(int button = 0; button < N_BUTTONS-1; button++){ // N_BUTTONS-1 = Exclude cab button
            int buttonPressed = elevio_callButton(floor, button);

            elevio_buttonLamp(floor, button, buttonPressed); //

            add_order(&elevator, floor)
        }
    }
};    




void check_cab_buttons() {
    for(int floor = 0; floor < N_FLOORS; floor++) {
        int buttonPressed = elevio_callButton(floor, BUTTON_CAB);
        elevio_buttonLamp(floor, BUTTON_CAB, buttonPressed);
    }
};


