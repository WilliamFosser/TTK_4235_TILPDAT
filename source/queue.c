#include "queue.h"
#include <stdbool.h>
#include "driver/elevio.h"


Queue new_queue() {
    Queue queue = {
        .prioritized_orders = {false, false, false, false},
        .unprioritized_orders = {false, false, false, false}
    };
    return queue;
}

void add_order(Queue *queue, uint8_t floor, bool is_cab_order) {
    if (is_cab_order) {
        queue->prioritized_orders[floor] = true;
    } else {
        queue->unprioritized_orders[floor] = true;
    }
};


void pop_order(Queue *queue, uint8_t floor) {
    queue->prioritized_orders[floor] = false;
    queue->unprioritized_orders[floor] = false;
}; 


void pop_all_orders(Queue *queue) {
    for (uint8_t i = 0; i < N_FLOORS; i++) {
        queue->prioritized_orders[i] = false;
        queue->unprioritized_orders[i] = false;
    }
};