#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "driver/elevio.h"
#include "queue.h"

typedef MotorDirection Direction; 

/**
 * @file
 * @brief Structure representing the elevator.
 */
typedef struct
{
    int8_t floor; ///< Current floor of the elevator, -1 for undefined.
    Direction direction; ///< Current direction of the elevator.
    uint8_t last_floor; ///< Last known floor of the elevator.
    Direction last_direction; ///< Last direction of the elevator.
    bool stop_button; ///< State of the stop button.
    bool door_open; ///< State of the door.
    bool obstructed; ///< State of the obstruction sensor.
    bool isIdle; ///< State indicating if the elevator is idle.
    int nextFloor; ///< Next floor the elevator will move to.
    bool stopFlag; ///< Flag indicating if the stop button was pressed.
    Queue queue; ///< Queue of orders for the elevator.
} Elevator;

/**
 * @brief Initializes the elevator.
 * 
 * @param elevator Pointer to the Elevator structure.
 */
void elevator_init(Elevator *elevator);

/**
 * @brief Elevator state machine.
 * 
 * @param elevator Pointer to the Elevator structure.
 */
void elevator_state_machine(Elevator *elevator);

/**
 * @brief Checks i@filef the elevator should stop at the current floor.
 * 
 * @param elevator Pointer to the Elevator structure.
 * @return true If the elevator should stop.
 * @return false If the elevator should not stop.
 */
bool check_if_stop(Elevator *elevator);

/**
 * @brief Stops the elevator and opens the door.
 * 
 * @param elevator Pointer to the Elevator structure.
 */
void stop_elevator(Elevator *elevator);

/**
 * @brief Updates the current floor of the elevator.
 * 
 * @param elevator Pointer to the Elevator structure.
 */
void update_floor(Elevator *elevator);

/**
 * @brief Opens the elevator door.
 * 
 * @param elevator Pointer to the Elevator structure.
 */
void open_door(Elevator *elevator);

/**
 * @brief Closes the elevator door.
 * 
 * @param elevator Pointer to the Elevator structure.
 */
void close_door(Elevator *elevator); 

/**
 * @brief Sets the direction of the elevator.
 * 
 * @param elevator Pointer to the Elevator structure.
 * @param direction The direction to set.
 */
void set_direction(Elevator *elevator, Direction direction);

/**
 * @brief Checks the state of the buttons and updates the elevator queue.
 * 
 * @param elevator Pointer to the Elevator structure.
 */
void check_buttons(Elevator *elevator);

/**
 * @brief Determines the direction the elevator should move.
 * 
 * @param elevator Pointer to the Elevator structure.
 * @return Direction The direction the elevator should move.
 */
Direction move_elevator(Elevator *elevator);

/**
 * @brief Handles the stop button functionality.
 * 
 * @param elevator Pointer to the Elevator structure.
 * @return true If the stop button was handled.
 * @return false If the stop button was not handled.
 */
bool handle_stop_butn(Elevator *elevator);