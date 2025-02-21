#include "elevator.h"
#include "queue.h"
#include "driver/elevio.h"
#include "timer.h"
#include <stdbool.h>

Elevator elevator_init() {
    Elevator elevator = {
        .floor = -1, // Assume undefined floor at initialization
        .direction = DIRN_STOP,
        .stop_button = false,
        .door_open = false,
        .obstructed = false,
        .queue = new_queue(),

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
void update_floor(Elevator *elevator) {
    elevator->floor = elevio_floorSensor();
    if (elevator->floor != -1) {
        elevator->last_floor = elevator->floor;
        elevio_floorIndicator(elevator->floor);
    }
};



void open_door() {
    elevio_doorOpenLamp(1);
};


void close_door() {
    elevio_doorOpenLamp(0);
};


// Check if any hall buttons are pressed and update the button lamps and the elevator queue
void check_hall_buttons(Elevator *elevator) {
    bool is_cab_order = false;
    for(int floor = 0; floor < N_FLOORS; floor++){
        for(int button = 0; button < N_BUTTONS-1; button++){ // N_BUTTONS-1 = Exclude cab button
            int buttonPressed = elevio_callButton(floor, button);
            elevio_buttonLamp(floor, button, buttonPressed);
            
            if (buttonPressed) {
                add_order(&(elevator->queue), floor, is_cab_order);
            }
        }
    }
}; 


void check_cab_buttons(Elevator *elevator) {
    bool is_cab_order = true;
    for(int floor = 0; floor < N_FLOORS; floor++) {
        int buttonPressed = elevio_callButton(floor, BUTTON_CAB);
        elevio_buttonLamp(floor, BUTTON_CAB, buttonPressed);

        if (buttonPressed) {
            add_order(&(elevator->queue), floor, is_cab_order);
        }
    }
};


void repreoritize_orders(Elevator *elevator) {
    uint8_t search_start = elevator->last_floor + elevator->direction;
    uint8_t search_end = search_start;
    switch (elevator->direction)
    {
    case DIRN_UP:
        while (!(elevator->queue.prioritized_orders[search_end])) {
            search_end++;
            if (search_end > N_FLOORS) {
                search_end = search_start;
                break;
            }
        }
        break;
    
    case DIRN_DOWN:
        while (!(elevator->queue.prioritized_orders[search_end])) {
            search_end--;
            if (search_end < 0) {
                search_end = search_start;
                break;
            }
        }
        break;

    default:
        break;
    }
    if (search_start <= search_end) {
        for (uint8_t i = search_start; i <= search_end; i++) {
            if (elevator->queue.unprioritized_orders[i]) {
                elevator->queue.prioritized_orders[i] = true;
                elevator->queue.unprioritized_orders[i] = false;
            }
        }
    } else {
        for (uint8_t i = search_start; i > search_end; i--) {
            if (elevator->queue.unprioritized_orders[i]) {
                elevator->queue.prioritized_orders[i] = true;
                elevator->queue.unprioritized_orders[i] = false;
            }
        }
    }
};

void move_elevator(Elevator *elevator) {
    //TODO: Implement move_elevator function
    // This function should move the elevator to the next floor in the queue


};

