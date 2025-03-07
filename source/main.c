
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include "driver/elevio.h"

#include "timer.h"
#include "elevator.h"







int main(){


    printf("=== Elevator starting..... ===\n");
    printf("Press the stop button on the elevator panel to exit\n");

    
    elevio_init();
    Elevator elevator;
    elevator_init(&elevator);
    elevator_state_machine(&elevator);


    printf("=== Elevator shutting down ===\n");

    return 0;
}
