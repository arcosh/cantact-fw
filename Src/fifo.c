
#include "fifo.h"


void fifo_init(fifo_t* fifo, uint8_t* buffer, uint16_t size)
{
    fifo->buffer = buffer;
    fifo->size = size;

    // Reset byte indices
    fifo->push_index = 0;
    fifo->pop_index = 0;
}


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



bool fifo_is_empty(fifo_t* fifo)
{
    // When the push_index is shifted against the pop_index, data is available for popping.
    return (fifo->pop_index == fifo->push_index);
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


bool fifo_has_slcan_command(fifo_t* fifo, uint16_t* length)
{
    *length = 0;
    if (fifo_is_empty(fifo))
        return false;

    uint16_t l = fifo_get_length(fifo);
    for (uint16_t i=0; i<l; i++)
    {
        uint16_t index = fifo->pop_index + i;
        if (index >= fifo->size)
            index -= fifo->size;
        if (fifo->buffer[index] == SLCAN_COMMAND_TERMINATOR)
        {
            *length = i+1;
            return true;
        }
    }

    return false;
}


bool fifo_push(fifo_t* fifo, uint8_t* data, uint16_t length)
{
    if (!fifo_has_room(fifo, length))
        // There is not enough free space in the buffer.
        return false;

    for (uint16_t i=0; i<length; i++)
    {
        // Copy byte to buffer
        fifo->buffer[fifo->push_index] = data[i];

        // Increment the push_index
        fifo->push_index = (fifo->push_index + 1) % fifo->size;
    }
    return true;
}


bool fifo_pop(fifo_t* fifo, uint8_t* data, uint16_t length)
{
    if (fifo_get_length(fifo) < length)
        // There is fewer bytes in the buffer than requested.
        return false;

    for (uint16_t i=0; i<length; i++)
    {
        // Copy byte from buffer
        data[i] = fifo->buffer[fifo->pop_index];

        // Increment the pop_index
        fifo->pop_index = (fifo->pop_index + 1) % fifo->size;
    }
    return true;
}


bool fifo_get(fifo_t* fifo, uint8_t* data, uint16_t length)
{
    if (fifo_get_length(fifo) < length)
        // There is fewer bytes in the buffer than requested.
        return false;

    // Make a copy of the pop index, don't touch the original
    uint16_t pop_index = fifo->pop_index;

    for (uint16_t i=0; i<length; i++)
    {
        // Copy byte from buffer
        data[i] = fifo->buffer[pop_index];

        // Increment the pop_index
        pop_index = (pop_index + 1) % fifo->size;
    }
    return true;
}


bool fifo_drop_oldest(fifo_t* fifo, uint16_t length)
{
    if (fifo_get_length(fifo) < length)
        // There is fewer bytes in the buffer than you wish to delete.
        return false;

    fifo->pop_index = (fifo->pop_index + length) % fifo->size;
    return true;
}
