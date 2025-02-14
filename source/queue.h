#pragma once

#include <stdint.h>

typedef struct 
{
    bool prioritized_orders[4] = {false, false, false, false};
    bool unprioritized_orders[4]= {false, false, false, false};
} Queue;


Queue new_queue();
void add_order(uint8_t); 
void pop_order(uint8_t);


