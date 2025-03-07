#include "elevator.h"
#include "queue.h"
#include "driver/elevio.h"
#include <stdio.h>
#include <time.h>
#include "timer.h"

Timer timer;

void elevator_state_machine(Elevator *elevator) {
    while(1) {
        update_floor(elevator);
        check_hall_buttons(elevator);
        check_cab_buttons(elevator);
        
        // Handle stop button. Dont handle obstruction while stop button active?
        while (elevio_stopButton()) {
            elevator->stop_flag = true;
            elevio_stopLamp(1);
            set_direction(elevator, DIRN_STOP);

            pop_all_orders(&elevator->queue);

            if (elevator->floor != -1) {
                open_door(elevator);
            }
        }

        elevio_stopLamp(0);

        if (elevator->door_open && elevator->stop_flag) {
            elevator->stop_flag = false;
            start_timer(&timer, 3);
            
            while (!timer_expired(&timer)) {
                // Wait for 3 seconds
                check_hall_buttons(elevator);
                check_cab_buttons(elevator);
            }
            close_door(elevator);
        }
        
        

        printf("Prioritized [");
        //reprioritize_orders(elevator);
        for(int i = 0; i<N_FLOORS; i++){
            printf(",%d",elevator->queue.prioritized_orders[i]);
        }
        printf("]\n\n");

        reprioritize_orders(elevator);
        move_elevator(elevator);
    

        //TODO! Implement "move_elevator" function
        
        nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
    }
};

