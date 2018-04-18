
#ifndef FIFO_H
#define FIFO_H

#include <stdint.h>
#include <stdbool.h>
#include "slcan.h"


/**
 * First-in first out buffer meta-information struct
 */
typedef struct {
    /**
     * If you wish to use fifo_push or fifo_pop
     * on this FIFO, you must initialize this pointer.
     */
    uint8_t* buffer;

    /**
     * Total number of element slots this FIFO can hold
     */
    uint16_t size;

    /**
     * Number of completely pushed elements in the buffer
     *
     * One element can consist of several bytes
     * (e.g. number of complete SLCAN frames).
     */
    uint16_t element_count;

    /**
     * The index at which to store incoming data
     */
    volatile uint16_t push_index;

    /**
     * The index of the next element to pop from the FIFO
     */
    volatile uint16_t pop_index;
} fifo_t;


/**
 * Initialize FIFO buffer
 */
inline void fifo_init(fifo_t* fifo, uint8_t* buffer, uint16_t size)
{
    fifo->buffer = buffer;
    fifo->size = size;
    fifo->element_count = 0;
    fifo->push_index = 0;
    fifo->pop_index = 0;
}

/**
 * Returns whether the buffer has any data stored or not
 */
inline bool fifo_is_empty(fifo_t* fifo)
{
    // When the push_index is shifted against the pop_index, data is available for popping.
    return (fifo->pop_index == fifo->push_index);
}

/**
 * Returns the number of bytes currently stored in the buffer
 */
uint16_t fifo_get_length(fifo_t* fifo);

/**
 * Returns the number of bytes which could currently be pushed to the buffer
 */
uint16_t fifo_get_remaining(fifo_t* fifo);

/**
 * Returns whether there is empty space available in the buffer or not
 */
bool fifo_has_room(fifo_t* fifo, uint8_t length);

/**
 * Append data to the buffer
 *
 * @param fifo      Buffer to push data to
 * @param string    Data to push to the buffer
 * @param length    Number of bytes to push to the buffer
 */
void fifo_push(fifo_t* fifo, uint8_t* data, uint16_t length);

/**
 * Copy and delete data from the buffer
 *
 * @param fifo      Buffer to retrieve data from
 * @param data      Buffer to write data to
 * @param length    Number of bytes to retrieve from the buffer
 */
void fifo_pop(fifo_t* fifo, uint8_t* data, uint16_t length);

/**
 * Copy data from the buffer (without deleting it)
 *
 * @param fifo      Buffer to retrieve data from
 * @param data      Buffer to write data to
 * @param length    Number of bytes to retrieve from the buffer
 */
void fifo_copy(fifo_t* fifo, uint8_t* buffer, uint16_t length);

/**
 * Discard number of bytes from the buffer
 *
 * @param fifo      FIFO to discard bytes from
 * @param size      Number of bytes to discard
 */
void fifo_discard(fifo_t* fifo, uint16_t size);

#endif
