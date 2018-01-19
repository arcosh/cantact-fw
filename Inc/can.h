/**
 * @file
 * @brief Header file for the CAN-related functions implemented in @ref can.c
 */

#ifndef _CAN_H
#define _CAN_H

#include "stm32f0xx_hal.h"
#include "can_timing.h"

/**
 * List of supported bitrates on the CAN bus
 */
enum can_bitrate {
    CAN_BITRATE_10K,
    CAN_BITRATE_20K,
    CAN_BITRATE_50K,
    CAN_BITRATE_100K,
    CAN_BITRATE_125K,
    CAN_BITRATE_250K,
    CAN_BITRATE_500K,
    CAN_BITRATE_750K,
    CAN_BITRATE_1000K,
};

/**
 * List of possible CAN bus states
 */
enum can_bus_state {
    OFF_BUS,
    ON_BUS
};

/**
 * Prepare CAN bus peripheral for initialization
 */
void can_init(void);

/**
 * Initialize CAN peripheral and clear CAN ID filters
 */
void can_enable(void);

/**
 * Disable CAN peripheral by performing a CAN peripheral reset
 */
void can_disable(void);

/**
 * Set the bitrate of the CAN bus
 */
void can_set_bitrate(enum can_bitrate bitrate);

/**
 * Enable monitoring the CAN bus chatter for frames with a specific CAN ID or mask
 */
void can_set_filter(uint32_t id, uint32_t mask);

/**
 * Enqueue a frame for transmission
 */
void can_send(CanTxMsgTypeDef* frame);

/**
 * Set the CAN operating mode to silent, i.e. disable transmissions
 */
void can_set_silent(uint8_t silent);

/**
 * Aborts transmissions with errors
 */
void can_check_transmit_mailboxes();

/**
 * Perform all CAN-related tasks in the queue
 */
void can_process();

#endif // _CAN_H
