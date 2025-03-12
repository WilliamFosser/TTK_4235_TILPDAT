#include "elevator.h"
#include "queue.h"
#include "driver/elevio.h"
#include <stdio.h>
#include <time.h>
#include "timer.h"

void elevator_state_machine(Elevator *elevator) {
    while(1) {
        if (elevator->door_open) {
            close_door(elevator);
        }
        update_floor(elevator);
        check_if_stop(elevator);
        check_buttons(elevator);
        handle_stop_butn(elevator);
        move_elevator(elevator);
        check_if_stop(elevator);
        printf("Current floor: %d\n", elevator->floor);
        
        nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
    }
};

