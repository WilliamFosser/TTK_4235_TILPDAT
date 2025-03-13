
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include "driver/elevio.h"
#include "driver/con_load.h"

#include "timer.h"
#include "elevator.h"


int main() {

    uint32_t loop_rate_ms = 10;
    con_load("source/elevator.con", con_val("loop_rate_ms", &loop_rate_ms, "%d"));
    
    printf("elevator.con loaded\n\n");
    printf("Loop rate: %d ms\n", loop_rate_ms);
    printf("=== Elevator starting..... ===\n");

    elevio_init();
    elevator_init();

    while(1) {
        update_floor();
        check_call_buttons();
        
       

        elevator_state_machine();



        usleep(loop_rate_ms * 1000); // Microseconds to milliseconds
    }

    printf("=== Elevator shutting down ===\n");

    return 0;
}
