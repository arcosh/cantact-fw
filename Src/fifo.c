
#include "stm32f0xx_hal.h"
#include "fifo.h"
#include "can.h"


void fifo_init(fifo_t* fifo) {
    fifo->push_index = 0;
    fifo->pop_index = 0;
}


bool fifo_is_full(fifo_t* fifo) {
    // The buffer is full, when the next push would increment the push_index to the pop_index.
    return ((fifo->push_index + 1) % FIFO_SIZE == fifo->pop_index);
}


bool fifo_is_empty(fifo_t* fifo) {
    // When the push_index is shifted against the pop_index, data is available for popping.
    return (fifo->pop_index == fifo->push_index);
}


void fifo_push(
        fifo_t* fifo,
        uint32_t StdId,
        uint32_t ExtId,
        uint32_t IDE,
        uint32_t RTR,
        uint32_t DLC,
        uint32_t* Data
        ) {

    if (fifo_is_full(fifo))
        return;

    // Cache the current push_index
    uint16_t index = fifo->push_index;

    // Copy frame to buffer
    fifo->buffer[index].StdId = StdId;
    fifo->buffer[index].ExtId = ExtId;
    fifo->buffer[index].IDE = IDE;
    fifo->buffer[index].RTR = RTR;
    fifo->buffer[index].DLC = DLC;
    for (uint8_t i=0; i<8; i++) {
        fifo->buffer[index].Data[i] = Data[i];
    }

    // Increment the push_index
    fifo->push_index = (fifo->push_index + 1) % FIFO_SIZE;
}


void fifo_get_oldest_entry(
        fifo_t* fifo,
        uint32_t* StdId,
        uint32_t* ExtId,
        uint32_t* IDE,
        uint32_t* RTR,
        uint32_t* DLC,
        uint32_t* Data
        ) {

    // Cache the current pop_index
    uint16_t index = fifo->pop_index;

    // Copy frame from buffer
    *StdId = fifo->buffer[index].StdId;
    *ExtId = fifo->buffer[index].ExtId;
    *IDE = fifo->buffer[index].IDE;
    *RTR = fifo->buffer[index].RTR;
    *DLC = fifo->buffer[index].DLC;
    for (uint8_t i=0; i<8; i++) {
        Data[i] = fifo->buffer[index].Data[i];
    }
}


void fifo_drop_oldest_entry(fifo_t* fifo) {
    if (fifo_is_empty(fifo))
        return;

    // It is not necessary to actually delete the old data, since it will be overwritten by the next push anyway.
    fifo->pop_index = (fifo->pop_index + 1) % FIFO_SIZE;
    uint32_t a = fifo->pop_index;
    a++;
}
