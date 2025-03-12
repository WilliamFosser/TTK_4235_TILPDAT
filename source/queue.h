#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "driver/elevio.h"

typedef struct 
{
    int* orders[4];
    int order_type[4];
} Queue;


void new_queue(Queue *queue);
void add_order(Queue *queue, uint8_t floor, ButtonType button_type);
void pop_order(Queue *queue, uint8_t floor);
void pop_all_orders(Queue *queue);


