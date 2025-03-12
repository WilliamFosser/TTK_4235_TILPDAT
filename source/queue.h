#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef MotorDirection Direction; 

void add_order(uint8_t floor, ButtonType button);
void pop_order(uint8_t floor);
void pop_all_orders();


