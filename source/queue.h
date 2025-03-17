#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "driver/elevio.h"

/**
 * @file
 * @brief Structure representing the queue 
 * as an array of bools. First index = floor, secound index = ButtonType
 */
typedef struct 
{
    bool orders[N_FLOORS][N_BUTTONS]; ///< [Floor][ButtonType], ButtonType = {BUTTON_HALL_UP, BUTTON_HALL_DOWN, BUTTON_CAB}.
} Queue;


/**
 * @brief Adds an order to the queue.
 * 
 * @param queue Pointer to the Queue structure.
 * @param floor The floor for the order.
 * @param button_type The type of button pressed.
 */
void add_order(Queue *queue, uint8_t floor, ButtonType button_type);


/**
 * @brief Removes an order from the queue.
 * 
 * @param queue Pointer to the Queue structure.
 * @param floor The floor-order to remove.
 */
void pop_order(Queue *queue, uint8_t floor);


/**
 * @file
 * @brief Clears all orders from the queue at all floors.
 * 
 * @param queue Pointer to the Queue structure.
 */
void pop_all_orders(Queue *queue);


