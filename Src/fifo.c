
#include "fifo.h"


bool fifo_has_room(fifo_t* fifo, uint8_t length)
{
    // The buffer can't store more than buffer size-1 bytes.
    if (length+1 >= fifo->size)
        return false;

    // The buffer is full, when pushing any amount of bytes between 1 and length+1
    // would increment the push_index to match the pop_index.
    for (uint16_t l=1; l<=length+1; l++)
    {
        if ((fifo->push_index + l) % fifo->size == fifo->pop_index)
        {
            return false;
        }
    }
    return true;
}


uint16_t fifo_get_length(fifo_t* fifo)
{
    if (fifo->push_index < fifo->pop_index)
    {
        return (fifo->push_index + fifo->size) - fifo->pop_index;
    }
    if (fifo->push_index > fifo->pop_index)
    {
        return fifo->push_index - fifo->pop_index;
    }
    // fifo->push_index == fifo->pop_index: Buffer is empty
    return 0;
}


bool fifo_push(fifo_t* fifo, uint8_t* data, uint16_t length)
{
    for (uint16_t i=0; i<length; i++)
    {
        // Copy byte to buffer
        fifo->buffer[fifo->push_index] = data[i];

        // Increment the push_index
        uint16_t j = fifo->push_index + 1;
        while (j >= fifo->size)
        {
            j -= fifo->size;
        }
        fifo->push_index = j;
    }
    return true;
}


void fifo_pop(fifo_t* fifo, uint8_t* data, uint16_t length)
{
    for (uint16_t i=0; i<length; i++)
    {
        // Copy byte from buffer
        data[i] = fifo->buffer[fifo->pop_index];

        // Increment the pop_index
        uint16_t j = fifo->pop_index + 1;
        while (j >= fifo->pop_index)
        {
            j -= fifo->size;
        }
        fifo->pop_index = j;
    }
}


void fifo_copy(fifo_t* fifo, uint8_t* buffer, uint16_t length)
{
    uint16_t j = fifo->pop_index;

    for (uint16_t i=0; i<length; i++)
    {
        // Copy byte from buffer
        data[i] = fifo->buffer[j++];

        while (j >= fifo->size)
        {
            j -= fifo->size;
        }
    }
}


void fifo_discard(fifo_t* fifo, uint16_t size)
{
    // Copy value to temporary variable to avoid problems with interrupts reading/writing the pop_index
    uint16_t j = fifo->pop_index;
    j += size;
    while (j >= fifo->size)
    {
        j -= fifo->size;
    }
    fifo->pop_index = j;
}
