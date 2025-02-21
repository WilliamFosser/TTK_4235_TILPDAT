#include "elevator.h"
#include "queue.h"
#include "driver/elevio.h"
#include <stdio.h>
#include <time.h>

void elevator_state_machine(Elevator *elevator) {

    while(1) {
        update_floor(elevator);
        check_hall_buttons(elevator);
        check_cab_buttons(elevator);
        repreoritize_orders(elevator);

        if(elevio_obstruction()){
            elevio_stopLamp(1);
            printf("Obstruction detected\n");
        } else {
            elevio_stopLamp(0);
        }
        
        if(elevio_stopButton()){
            elevio_motorDirection(DIRN_STOP);
            break;
        };


        //TODO! Implement "move_elevator" function
        
        nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
    }
};


