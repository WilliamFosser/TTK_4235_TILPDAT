#include "elevator.h"
#include "queue.h"
#include "driver/elevio.h"
#include "timer.h"
#include <stdbool.h>
#include <stdlib.h>

Elevator* elevator_init() {
    Elevator* elevator = malloc(sizeof(Elevator));
    if (elevator == NULL) {
        // Håndter allokeringsfeil
        return NULL;
    }

    elevator->floor = -1; // Assume undefined floor at initialization
    elevator->direction = DIRN_STOP;
    elevator->stop_button = false;
    elevator->door_open = false;
    elevator->obstructed = false;
    elevator->queue = new_queue();

    if (elevio_floorSensor() != -1) {
        elevator->floor = elevio_floorSensor();
    } else {
        set_direction(elevator, DIRN_DOWN);
        while (elevio_floorSensor() == -1) {
            // Wait for elevator to reach a floor
        }
        elevator->floor = elevio_floorSensor();
        set_direction(elevator, DIRN_STOP);    
    }
    return elevator;
}


// Returns the current floor of the elevator. 
// If the elevator is between floors, the function returns -1.
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
    if (elevio_obstruction()) {
        return;
    }
    else {
    elevio_doorOpenLamp(0);
    elevator->door_open = false;
    }
    set_direction(elevator, elevator->last_direction);
};


// Check if any hall buttons are pressed and update the button lamps and the elevator queue
void check_hall_buttons(Elevator *elevator) {
    bool is_cab_order = false;
    for(int floor = 0; floor < N_FLOORS; floor++){
        for(int button = 0; button < N_BUTTONS-1; button++){ // N_BUTTONS-1 = Exclude cab button
            int buttonPressed = elevio_callButton(floor, button);
            elevio_buttonLamp(floor, button, buttonPressed);
            
            if (buttonPressed) {
                add_order(elevator->queue, floor, is_cab_order);
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
            add_order(elevator->queue, floor, is_cab_order);
        }
    }
};


void repreoritize_orders(Elevator *elevator) {
    uint8_t search_start = elevator->last_floor + elevator->direction;
    uint8_t search_end = search_start;
    switch (elevator->direction)
    {
    case DIRN_UP:
        while (!(elevator->queue->prioritized_orders[search_end])) {
            search_end++;
            if (search_end > N_FLOORS) {
                search_end = search_start;
                break;
            }
        }
        break;
    
    case DIRN_DOWN:
        while (!(elevator->queue->prioritized_orders[search_end])) {
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
            if (elevator->queue->unprioritized_orders[i]) {
                elevator->queue->prioritized_orders[i] = true;
                elevator->queue->unprioritized_orders[i] = false;
            }
        }   
    }
        
    else {
        for (uint8_t i = search_start; i > search_end; i--) {
            if (elevator->queue->unprioritized_orders[i]) {
                elevator->queue->prioritized_orders[i] = true;
                elevator->queue->unprioritized_orders[i] = false;
            }
        }
    }
};

void set_direction(Elevator *elevator, int8_t direction) {
    if (direction != DIRN_STOP) {
        elevator->last_direction = elevator->direction;
    }
    elevator->direction = direction;
    elevio_motorDirection(elevator->direction);
};

void move_elevator(Elevator *elevator) {
    //TODO: Implement move_elevator function
    // This function should move the elevator to the next floor in the queue
    if (!elevator->door_open) {
        set_direction(elevator, elevator->last_direction);
        if (elevator->floor == -1) {
            int8_t check_start = elevator->last_floor + elevator->last_direction;
            for(; check_start > 0 || check_start < N_FLOORS; check_start+=(elevator->last_direction)) {
                if (elevator->queue->prioritized_orders[check_start]) {
                    set_direction(elevator, elevator->last_direction);
                }
                else {
                    set_direction(elevator, (elevator->last_direction) * (-1));
                }
            }
        }
        else {
            if (elevator->queue->prioritized_orders[elevator->floor]) {
                set_direction(elevator, DIRN_STOP);
                open_door(elevator);
                pop_order(elevator->queue, elevator->floor);
            }
        }
    

    }
};

