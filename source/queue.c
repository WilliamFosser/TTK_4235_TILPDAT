#include "queue.h"
#include <stdbool.h>
#include "driver/elevio.h"
#include <stdio.h>


void new_queue(Queue *queue) {
    for (uint8_t i = 0; i < N_FLOORS; i++) {
        queue->prioritized_orders[i] = false;
        queue->unprioritized_orders[i] = false;
    }
};

void add_order(Queue *queue, uint8_t floor, bool is_cab_order) {
    if (is_cab_order) {
        queue->prioritized_orders[floor] = true;
        printf("Added prioritized order \n");
    } else {
        queue->unprioritized_orders[floor] = true;
        printf("Added unprioritized order \n");
    }
};


void pop_order(Queue *queue, uint8_t floor) {
    queue->prioritized_orders[floor] = false;
    queue->unprioritized_orders[floor] = false;

    elevio_buttonLamp(floor, BUTTON_HALL_DOWN, 0);
    elevio_buttonLamp(floor, BUTTON_HALL_UP, 0);
    elevio_buttonLamp(floor, BUTTON_CAB, 0);
}; 


void pop_all_orders(Queue *queue) {
    for (uint8_t floor = 0; floor < N_FLOORS; floor++) {
        queue->prioritized_orders[floor] = false;
        queue->unprioritized_orders[floor] = false;

        elevio_buttonLamp(floor, BUTTON_HALL_DOWN, 0);
        elevio_buttonLamp(floor, BUTTON_HALL_UP, 0);
        elevio_buttonLamp(floor, BUTTON_CAB, 0);
    }
};
    
    