#include "queue.h"
#include <stdbool.h>
#include "driver/elevio.h"
#include <stdio.h>

// Defined in .c file to make the interface minimal. Only one queue is needed. 
typedef struct 
{
    bool cab_orders[N_FLOORS];
    bool hall_up_orders[N_FLOORS];
    bool hall_down_orders[N_FLOORS];
} Queue;


static Queue queue = {
    .cab_orders = {false, false, false, false},
    .hall_up_orders = {false, false, false, false},
    .hall_down_orders = {false, false, false, false}
};


void add_order(uint8_t floor, ButtonType button) {
    switch (button) {
    case BUTTON_CAB:
        queue.cab_orders[floor] = true;
        printf("Added cab order on floor %d\n", floor);
        break;
        
    case BUTTON_HALL_UP:
        queue.hall_up_orders[floor] = true; 
        printf("Added hall up order on floor %d\n", floor);
        break;

    case BUTTON_HALL_DOWN:
        queue.hall_down_orders[floor] = true; 
        printf("Added hall down order on floor %d\n", floor);
        break;
        
    default:
        printf("Error: Invalid button type\n");
        break;
    }
}
    
// Product spesification specifies that when the elevator stops at a floor, all orders at that floor should be cleared. 
// (all persons should leave the elevator, and all persons waiting at the floor should enter the elevator)
// This function is called when the elevator stops at a floor.
void pop_order(uint8_t floor) {
    queue.cab_orders[floor] = false;
    queue.hall_up_orders[floor] = false;
    queue.hall_down_orders[floor] = false;

    clear_floor_lamps(floor);
}


void pop_all_orders() {
    for (uint8_t floor = 0; floor < N_FLOORS; floor++) {
        pop_order(floor);
    }
}


// Product spesification specifies that when the elevator stops at a floor, all orders at that floor should be cleared. 
// (all persons should leave the elevator, and all persons waiting at the floor should enter the elevator)
// This function is called when the elevator stops at a floor.
static void clear_floor_lamps(uint8_t floor) {
    elevio_buttonLamp(floor, BUTTON_HALL_DOWN, 0);
    elevio_buttonLamp(floor, BUTTON_HALL_UP, 0);
    elevio_buttonLamp(floor, BUTTON_CAB, 0);
}


bool orders_in_direction(Direction direction, uint8_t floor) {
    switch (direction) {
    case DIRN_UP:
        for (int order = floor; order < N_FLOORS; order++) {
            if (queue.cab_orders[order]     || 
                queue.hall_up_orders[order] || 
                queue.hall_down_orders[order]) {
                return true;
            }
        }
        break;
    case DIRN_DOWN:
        for (int order = floor; order >= 0; order--) {
            if (queue.cab_orders[order]     || 
                queue.hall_up_orders[order] || 
                queue.hall_down_orders[order]) {
                return true;
            }
        }
        break;
    default:
        printf("Error: Invalid direction\n");
        break;
    }
    return false;
}


bool is_order_opposite_direction(Direction direction, uint8_t floor) {
    switch (direction) {
    case DIRN_UP:
        if (queue.hall_down_orders[floor]) {
            return true;
        }
        break;
    case DIRN_DOWN:
        if (queue.hall_up_orders[floor]) {
            return true;
        }
        break;
    default:
        printf("Error: Invalid direction\n");
        break;
    }
    return false;
}