
#ifndef BUFFERING_H
#define BUFFERING_H

/**
 * Initialize all buffers
 */
void buffering_init();

/**
 * If at least one CAN frame is available
 * in the received frames buffer and
 * there is space in the serial buffer,
 * then the frame is converted and moved.
 */
void process_rx_frame_buffer();

/**
 * If at least one SLCAN frame is available
 * in the received frames serial buffer,
 * it pops it and writes it to the host via USB.
 */
void process_rx_serial_buffer();

/**
 * If a complete SLCAN frame is available
 * in the USB incoming serial buffer and
 * there is space in the CAN TX frame buffer,
 * the frame is converted and moved.
 */
void process_tx_serial_buffer();

/**
 * If there is at least one frame available
 * in the CAN TX frame buffer and at least one
 * of the three CAN TX mailboxes is empty,
 * the frame is popped and queued for transmission.
 */
void process_tx_frame_buffer();

#endif
