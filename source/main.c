
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include "driver/elevio.h"

#include "timer.h"
#include "elevator.h"




int main() {

    uint32_t loop_rate_ms;
    con_load("elevator.con", con_val("loop_rate_ms", &loop_rate_ms, "%d"));
    
    printf("elevator.con loaded\n\n");
    printf("Loop rate: %d ms\n", loop_rate_ms);

    printf("=== Elevator starting..... ===\n");
    printf("Press the stop button on the elevator panel to exit\n");

    elevator_init();

    while(1) {
        update_floor();
        check_call_buttons();



        usleep(loop_rate_ms * 1000); // Microseconds to milliseconds


    }

    
    elevator_init();
    elevator_state_machine(&elevator);


    printf("=== Elevator shutting down ===\n");

    return 0;
}
