#include "queue.h"
#include <stdbool.h>
#include "driver/elevio.h"
#include <stdio.h>


void new_queue(Queue *queue) {
    queue->order_type[0] = 1; //Up, Position = ButtonType
    queue->order_type[1] = -1; //Down
    queue->order_type[2] = 0; //Cab
    queue->order_type[3] = 100; //No order
    for (uint8_t i = 0; i < 4; i++) {
        queue->orders[i] = &(queue->order_type[3]);
    }
};

void add_order(Queue *queue, uint8_t floor, ButtonType button_type) {
    if(*(queue->orders[floor]) == 100) {
        queue->orders[floor] = &(queue->order_type[button_type]);
        elevio_buttonLamp(floor, button_type, 1);
    }
    else if (button_type == BUTTON_CAB) {
        queue->orders[floor] = &(queue->order_type[button_type]);
        elevio_buttonLamp(floor, button_type, 1);
        printf("Added cab order \n");} 
    else{
        if (*(queue->orders[floor]) != queue->order_type[button_type]) { //If there's an order in the opposite direction, order doesnt matter
            queue->orders[floor] = &(queue->order_type[2]);
            elevio_buttonLamp(floor, button_type, 1);
            printf("Added cab order \n");}
        else {
            queue->orders[floor] = &(queue->order_type[button_type]);
            elevio_buttonLamp(floor, button_type, 1);
            printf("Added down order  in direction %d\n", queue->order_type[button_type]);}
    }
};


void pop_order(Queue *queue, uint8_t floor) {
    queue->orders[floor] = &(queue->order_type[3]);

    elevio_buttonLamp(floor, BUTTON_HALL_DOWN, 0);
    elevio_buttonLamp(floor, BUTTON_HALL_UP, 0);
    elevio_buttonLamp(floor, BUTTON_CAB, 0);
}; 


void pop_all_orders(Queue *queue) {
    for (uint8_t floor = 0; floor < N_FLOORS; floor++) {
        queue->orders[floor] = &(queue->order_type[3]);

        elevio_buttonLamp(floor, BUTTON_HALL_DOWN, 0);
        elevio_buttonLamp(floor, BUTTON_HALL_UP, 0);
        elevio_buttonLamp(floor, BUTTON_CAB, 0);
    }
};
    
    