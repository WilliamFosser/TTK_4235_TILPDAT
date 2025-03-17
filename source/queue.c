#include "queue.h"
#include <stdbool.h>
#include "driver/elevio.h"
#include <stdio.h>

/**
 * @file
 * @brief Clears all orders from the queue.
 * 
 * @param queue Pointer to the Queue structure.
 */
void pop_all_orders(Queue *queue) {
    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < 3; j++) {
            elevio_buttonLamp(i, j, 0);
            queue->orders[i][j] = false;
        }
    }
};

/**
 * @brief Adds an order to the queue.
 * 
 * @param queue Pointer to the Queue structure.
 * @param floor The floor for the order.
 * @param button_type The type of button pressed.
 */
void add_order(Queue *queue, uint8_t floor, ButtonType button_type) {
    queue->orders[floor][button_type] = true;
};

/**
 * @brief Removes an order from the queue.
 * 
 * @param queue Pointer to the Queue structure.
 * @param floor The floor for the order to deleete.
 */
void pop_order(Queue *queue, uint8_t floor) {
    for (uint8_t i = 0; i < 3; i++) { //turn off lights for all buttons at floor
        queue->orders[floor][i] = false;
        elevio_buttonLamp(floor, i, 0);
    }
};



