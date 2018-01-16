
#ifndef FIFO_H
#define FIFO_H

#include <stdint.h>
#include <stdbool.h>


/**
 * Maximum number of elements in a FIFO
 */
#define FIFO_SIZE   7

typedef struct {
    uint32_t StdId;
    uint32_t ExtId;
    uint32_t IDE;
    uint32_t RTR;
    uint32_t DLC;
    uint32_t Data[8];
} can_frame_t;

typedef struct {
    volatile can_frame_t buffer[FIFO_SIZE];

    /**
     * Index of the position in which to store the next frame
     */
    volatile uint16_t push_index;

    /**
     * Index of the next frame to pop from the buffer
     */
    volatile uint16_t pop_index;
} fifo_t;


/**
 * Initialize empty FIFO buffer for CAN frames
 */
void fifo_init(fifo_t* fifo);

/**
 * Determine whether there is empty space in the buffer available or not
 */
bool fifo_is_full(fifo_t* fifo);

/**
 * Determine, whether a new frame is available or not
 */
bool fifo_is_empty(fifo_t* fifo);

/**
 * Append one frame to the buffer
 */
void fifo_push(
        fifo_t* fifo,
        uint32_t StdId,
        uint32_t ExtId,
        uint32_t IDE,
        uint32_t RTR,
        uint32_t DLC,
        uint32_t* Data
        );

/**
 * Retrieve one frame from the buffer
 */
void fifo_get_oldest_entry(
        fifo_t* fifo,
        uint32_t* StdId,
        uint32_t* ExtId,
        uint32_t* IDE,
        uint32_t* RTR,
        uint32_t* DLC,
        uint32_t* Data
        );

/**
 * Delete the oldest frame in the buffer
 */
void fifo_drop_oldest_entry(fifo_t* fifo);

#endif
