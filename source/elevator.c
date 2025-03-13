#include "elevator.h"
#include "queue.h"
#include "driver/elevio.h"
#include "timer.h"
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>


typedef struct
{
    int8_t floor;               // -1 for undefined 

    Direction direction; 
    Direction last_direction;   // Needed when the elevator is stopped, and is starting moving again

    ElevatorState state;
    bool door_open;
    int last_floor;

} Elevator;


static Elevator elevator = {
    .floor = -1,                // Assume undefined floor at initialization
    .direction = DIRN_STOP,
    .last_direction = DIRN_STOP,
    .state = STANDBY,
    .door_open = false,
    .last_floor = -1,
};


void elevator_init() {
    if (elevio_floorSensor() != -1) {
        elevator.floor = elevio_floorSensor();
    }
    else {
        set_direction(DIRN_DOWN);
        while (elevio_floorSensor() == -1) {
            // Wait for elevator to reach a floor
        }
        elevator.floor = elevio_floorSensor();
        set_direction(DIRN_STOP);
        elevio_floorIndicator(elevator.floor);
    }
    printf("Init finished\n");
};


void update_floor() {
    static int last_floor = -1;
    elevator.floor = elevio_floorSensor();
    if (elevator.floor != -1 && last_floor != elevator.floor) {
        elevio_floorIndicator(elevator.floor);
        elevator.last_floor = elevator.floor;
    }
    last_floor = elevator.floor;
}


void open_door() {
    set_direction(DIRN_STOP);
    elevator.state = DOOR_OPEN; // Håndtere døråpning i state machine? FJERNE DOOR_OPEN FRA ELEVATOR?
    elevator.door_open = true;
    //elevio_doorOpenLamp(1);
};

void close_door() {
    if (!elevio_obstruction()) {
        elevio_doorOpenLamp(0);
        // Set direction to last direction?
        set_direction(elevator.last_direction);
        elevator.door_open = false;
    }
};

void check_call_buttons() {
    for(int floor = 0; floor < N_FLOORS; floor++) {
        for(int button = 0; button < N_BUTTONS; button++) {
            int buttonPressed = elevio_callButton(floor, button);
            
            if (buttonPressed) {
                add_order(floor, button);
                print_queue();
                elevio_buttonLamp(floor, button, 1);
            }
        }
    }
}



void set_direction(Direction direction) {
    if (elevator.direction != DIRN_STOP) {
        elevator.last_direction = elevator.direction;
    }
    elevator.direction = direction;
    elevio_motorDirection(direction);
};

bool reached_end() {
    if (elevator.floor == 0 && elevator.direction == DIRN_DOWN) {
        return true;
    }
    if (elevator.floor == N_FLOORS-1 && elevator.direction == DIRN_UP) {
        return true;
    }
    return false;
};

static Direction opposite_direction(Direction direction) {
    if (direction == DIRN_UP) {
        return DIRN_DOWN;
    }
    else if (direction == DIRN_DOWN) {
        return DIRN_UP;
    }
    else {
        return DIRN_STOP;
    }
}



void elevator_state_machine() {

    printf("Current state: %d\n", elevator.state);
    //printf("Current floor: %d\n", elevator.floor);
    //printf("Current direction: %d\n", elevator.direction);


    if (elevio_stopButton()) {
        elevator.state = STOPPED;
    }

    switch (elevator.state) {
        case STANDBY:
            set_direction(DIRN_STOP);
            if (more_orders_in_direction(DIRN_UP, elevator.floor)) {
                set_direction(DIRN_UP);
                elevator.state = MOVING;
            } 
            else if (more_orders_in_direction(DIRN_DOWN, elevator.floor)) {
                set_direction(DIRN_DOWN);
                elevator.state = MOVING;
            }
            else {
                set_direction(DIRN_STOP);
            }
            break;

        case MOVING:
            if (!reached_end()) {
                if (order_at_floor(elevator.floor) && order_in_direction(elevator.direction, elevator.floor)) {
                    pop_order(elevator.floor);
                    elevator.state = DOOR_OPEN;
                }
                else if (!more_orders_in_direction(elevator.direction, elevator.last_floor)) {
                    if (more_orders_in_direction(opposite_direction(elevator.direction), elevator.last_floor)) {
                        set_direction(opposite_direction(elevator.direction));
                    }
                    else {
                        set_direction(DIRN_STOP);
                        elevator.state = STANDBY;
                    }
                }
            }
            else { // Heisen har nådd en ende
                if (order_at_floor(elevator.floor)) {
                    pop_order(elevator.floor);
                    open_door();
                }
                else if (more_orders_in_direction(opposite_direction(elevator.direction), elevator.last_floor)) {
                    set_direction(opposite_direction(elevator.direction));
                }
                else {
                    set_direction(DIRN_STOP);
                    elevator.state = STANDBY;
                }
            }
            break;

        case DOOR_OPEN: {
            static bool timer_active = false;
            if (!elevator.door_open) {
                open_door();
            }
            // Dersom det er hindring (f.eks. en person i døråpningen), nullstill timeren
            if (elevio_obstruction()) {
                timer_active = false;
                break;
            }
            // Sørg for at dørlampen er på dersom etasjen er definert
            if (elevator.floor != -1) {
                elevio_doorOpenLamp(1);
            }
            // Start timeren dersom den ikke allerede er aktiv
            if (!timer_active) {
                start_timer(3);
                timer_active = true;
            }
            // Når timeren utløper, lukk døren og fortsett bevegelse
            if (timer_expired()) {
                close_door();
                elevator.state = MOVING;
                timer_active = false;
            }
            break;
        }

        case STOPPED:
            set_direction(DIRN_STOP);
            // Ved stopp skal døren åpnes dersom heisen er i en etasje (D3, S7)
            if (elevator.floor != -1) {
                elevio_doorOpenLamp(1);
                open_door();
            }
            // Vent til stoppknappen slippes, og sørg for at stopplyset er tent mens knappen trykkes (S6)
            elevio_stopLamp(1);
            while (elevio_stopButton()) {
                usleep(10000);
            }
            // Slett alle ubetjente bestillinger (S5)
            pop_all_orders();
            elevio_stopLamp(0);
            if (elevator.floor != -1) {
                elevator.state = DOOR_OPEN;
            }
            else {
                elevator.state = STANDBY;
            }

            break;
            
        default:
            break;
    }
}
