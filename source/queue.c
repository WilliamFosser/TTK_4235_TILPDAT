#include "queue.h"
#include <stdbool.h>
#include "driver/elevio.h"
#include <stdio.h>


void new_queue(Queue *queue) {
    queue->order_type[0] = 1; //Up, Position = ButtonType
    queue->order_type[1] = -1; //Down
    queue->order_type[2] = 0; //Cab
    for (uint8_t i = 0; i < 4; i++) {
        queue->orders[i] = NULL;
    }
};

void add_order(Queue *queue, uint8_t floor, ButtonType button_type, Direction direction) {
    elevio_buttonLamp(floor, button_type, 1);
    if(queue->orders[floor] == NULL) {
        queue->orders[floor] = &(queue->order_type[button_type]);
    }
    else if (button_type == BUTTON_CAB) {
        queue->orders[floor] = &(queue->order_type[button_type]);
        printf("Added cab order \n");
    } else{
        if (*(queue->orders[floor]) != queue->order_type[button_type]) { //If there's an order in the opposite direction, order doesnt matter
            queue->orders[floor] = &(queue->order_type[2]);
            printf("Added cab order \n");}
        else {
            queue->orders[floor] = &(queue->order_type[button_type]);
            printf("Added down order  in direction %d\n", queue->order_type[button_type]);}
};


void pop_order(Queue *queue, uint8_t floor) {
    queue->orders[floor] = NULL;

    elevio_buttonLamp(floor, BUTTON_HALL_DOWN, 0);
    elevio_buttonLamp(floor, BUTTON_HALL_UP, 0);
    elevio_buttonLamp(floor, BUTTON_CAB, 0);
}; 


void pop_all_orders(Queue *queue) {
    for (uint8_t floor = 0; floor < N_FLOORS; floor++) {
        queue->orders[floor] = NULL;

        elevio_buttonLamp(floor, BUTTON_HALL_DOWN, 0);
        elevio_buttonLamp(floor, BUTTON_HALL_UP, 0);
        elevio_buttonLamp(floor, BUTTON_CAB, 0);
    }
};
    
    