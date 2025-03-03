#include "elevator.h"
#include "queue.h"
#include "driver/elevio.h"
#include "timer.h"
#include <stdbool.h>
#include <stdio.h>
#include <time.h>


void elevator_init(Elevator *elevator) {
    elevator->floor = -1;                    // Assume undefined floor at initialization
    elevator->direction = DIRN_STOP;
    elevator->stop_button = false;
    elevator->door_open = false;
    elevator->obstructed = false;

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
        set_direction(elevator, DIRN_STOP);
    }
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


void open_door(Elevator *elevator) {
    elevio_doorOpenLamp(1);
    elevator->door_open = true;
};


void close_door(Elevator *elevator) {
    if (!elevio_obstruction()) {
        elevio_doorOpenLamp(0);
        elevator->door_open = false;
        set_direction(elevator, elevator->last_direction);
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
};




void reprioritize_orders(Elevator *elevator) {
    // Only proceed if we have a valid floor
    if (elevator->last_floor < 0 || elevator->last_floor >= N_FLOORS) {
        return;
    }
    
    // Calculate initial search position
    int8_t search_start = elevator->last_floor;
    int8_t search_end = search_start;
    bool found_priority_order = false;
    
    switch (elevator->direction) {
    case DIRN_UP:
        // Search upward for the next prioritized order
        search_end = search_start;
        for (int8_t i = search_start; i < N_FLOORS; i++) {
            if (elevator->queue.prioritized_orders[i]) {
                search_end = i;
                found_priority_order = true;
                break;
            }
        }
        
        // If no orders found above, don't change search_end
        if (!found_priority_order) {
            search_end = search_start;
        }
        break;
    
    case DIRN_DOWN:
        // Search downward for the next prioritized order
        search_end = search_start;
        for (int8_t i = search_start; i >= 0; i--) {
            if (elevator->queue.prioritized_orders[i]) {
                search_end = i;
                found_priority_order = true;
                break;
            }
        }
        
        // If no orders found below, don't change search_end
        if (!found_priority_order) {
            search_end = search_start;
        }
        break;

    case DIRN_STOP:
        // When stopped, prioritize orders at the current floor
        if (elevator->queue.unprioritized_orders[search_start]) {
            elevator->queue.prioritized_orders[search_start] = true;
            elevator->queue.unprioritized_orders[search_start] = false;
        }
        return;
    }
    
    // Move unprioritized orders to prioritized orders between start and end
    if (search_start <= search_end) {
        for (int8_t i = search_start; i <= search_end; i++) {
            if (elevator->queue.unprioritized_orders[i]) {
                elevator->queue.prioritized_orders[i] = true;
                elevator->queue.unprioritized_orders[i] = false;
            }
        }
    } else {
        for (int8_t i = search_start; i >= search_end; i--) {
            if (elevator->queue.unprioritized_orders[i]) {
                elevator->queue.prioritized_orders[i] = true;
                elevator->queue.unprioritized_orders[i] = false;
            }
        }
    }
}





//==============================================================================
//============================== Direction Algorithm ===========================
//==============================================================================


void move_elevator(Elevator *elevator) {
    //TODO: Implement move_elevator function
    // This function should move the elevator to the next floor in the queue
    if (!elevator->door_open) {
        set_direction(elevator, elevator->last_direction);
        if (elevator->floor == -1) {
            int8_t check_start = elevator->last_floor + elevator->last_direction;
            for(; check_start > 0 || check_start < N_FLOORS; check_start+=(elevator->last_direction)) {
                if (elevator->queue.prioritized_orders[check_start]) {
                    set_direction(elevator, elevator->last_direction);
                }
                else {
                    set_direction(elevator, (elevator->last_direction) * (-1));
                }
            }
        }
        else {
            if (elevator->queue.prioritized_orders[elevator->floor]) {
                set_direction(elevator, DIRN_STOP);
                open_door(elevator);
                pop_order(&elevator->queue, elevator->floor);
            }
        }
    

    }
};

