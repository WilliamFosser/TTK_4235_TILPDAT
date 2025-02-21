#pragma once

#include <stdint.h>

typedef struct 
{
    bool prioritized_orders[4];
    bool unprioritized_orders[4];
} Queue;


Queue new_queue();

void add_order(Queue *queue, uint8_t floor, bool is_cab_order);
void pop_order(Queue *queue, uint8_t floor);
void pop_all_orders(Queue *queue);


