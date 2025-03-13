#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "driver/elevio.h"

typedef MotorDirection Direction; 

void add_order(uint8_t floor, ButtonType button);
void pop_order(uint8_t floor);
void pop_all_orders();

bool more_orders_in_direction(Direction direction, uint8_t elevator_floor);
bool order_in_direction(Direction direction, uint8_t elevator_floor);
bool queue_empty();
bool order_at_floor(uint8_t floor);
void print_queue();


