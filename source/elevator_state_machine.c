#include "elevator.h"
#include "queue.h"
#include "driver/elevio.h"
#include <stdio.h>
#include <time.h>
#include "timer.h"

void elevator_state_machine(Elevator *elevator) {
    while(1) {
        if (elevator->door_open) {
            printf("Door is open\n");
            close_door(elevator);
        }

        update_floor(elevator);
        
        if(check_if_stop(elevator)){
            printf("Elevator stopping\n");
            stop_elevator(elevator);
        }
    
        handle_stop_butn(elevator);
        check_buttons(elevator);
        set_direction(elevator, move_elevator(elevator));
        if(elevio_floorSensor() == -1 && !elevator->stopFlag && elevator->direction == DIRN_STOP){
            set_direction(elevator, DIRN_DOWN);
        }
        printf("Current floor: %d\n. Door is: %d", elevator->floor, elevator->door_open);
        nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
    }
};

