#include "elevator.h"
#include "queue.h"
#include "driver/elevio.h"
#include <stdio.h>
#include <time.h>
#include "timer.h"

/**
 * @file
 * @brief Elevator state machine that controls the elevator's behavior. Main application loop.
 * 
 * @param elevator Pointer to the Elevator structure.
 */
void elevator_state_machine(Elevator *elevator) {
    while(1) {
        if (elevator->door_open) {
            close_door(elevator);
        }

        update_floor(elevator); 
        
        if(check_if_stop(elevator)){
            stop_elevator(elevator);
        }
    
        handle_stop_butn(elevator);
        check_buttons(elevator);
        set_direction(elevator, move_elevator(elevator));
        //Makes sure elevator starts after stop button is released
        if(elevio_floorSensor() == -1 && !elevator->stopFlag && elevator->direction == DIRN_STOP){ 
            set_direction(elevator, DIRN_DOWN);
        }
        nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
    }
};

