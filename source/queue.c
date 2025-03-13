#include "queue.h"
#include <stdbool.h>
#include "driver/elevio.h"
#include <stdio.h>


void pop_all_orders(Queue *queue) {
    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < 3; j++) {
            elevio_buttonLamp(i, j, 0);
            queue->orders[i][j] = false;
        }
    }
};

void add_order(Queue *queue, uint8_t floor, ButtonType button_type) {
    queue->orders[floor][button_type] = true;
};


void pop_order(Queue *queue, uint8_t floor) {
    for (uint8_t i = 0; i < 3; i++) {
        queue->orders[floor][i] = false;
        elevio_buttonLamp(floor, i, 0);
    }
}; 


    
    