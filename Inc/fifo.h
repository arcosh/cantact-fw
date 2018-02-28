
#ifndef FIFO_H
#define FIFO_H

#include <stdint.h>
#include <stdbool.h>
#include "slcan.h"


typedef struct {
    /**
     * Pointer to data buffer
     */
    uint8_t* buffer;

    /**
     * Total size of the linked buffer
     */
    uint16_t size;

    /**
     * At which byte index to store incoming data
     */
    volatile uint16_t push_index;

    /**
     * Index of the next byte to pop from the buffer
     */
    volatile uint16_t pop_index;
} fifo_t;


/**
 * Initialize FIFO buffer
 */
void fifo_init(fifo_t* fifo, uint8_t* buffer, uint16_t size);

/**
 * Returns whether there is empty space available in the buffer or not
 */
bool fifo_has_room(fifo_t* fifo, uint8_t length);

/**
 * Returns whether the buffer has any data stored or not
 */
bool fifo_is_empty(fifo_t* fifo);

/**
 * Returns number of bytes currently stored in the buffer
 */
uint16_t fifo_get_length(fifo_t* fifo);

/**
 * Returns whether the buffer contains at least one complete SLCAN command
 * i.e. a string terminated by a carriage return character
 */
bool fifo_has_slcan_command(fifo_t* fifo, uint16_t* length);

/**
 * Append data to the buffer
 *
 * @param fifo      Buffer to push data to
 * @param string    Data to push to the buffer
 * @param length    Number of bytes to push to the buffer
 */
bool fifo_push(fifo_t* fifo, uint8_t* data, uint16_t length);

/**
 * Retrieve data from the buffer
 *
 * @param fifo      Buffer to retrieve data from
 * @param data      Buffer to write data to
 * @param length    Number of bytes to retrieve from the buffer
 */
bool fifo_pop(fifo_t* fifo, uint8_t* data, uint16_t length);

bool fifo_get(fifo_t* fifo, uint8_t* data, uint16_t length);

bool fifo_drop_oldest(fifo_t* fifo, uint16_t length);

#endif
