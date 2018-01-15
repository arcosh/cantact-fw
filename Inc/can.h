/**
 * @file
 * @brief Header file for the CAN-related functions implemented in @ref can.c
 */

#ifndef _CAN_H
#define _CAN_H

#include "stm32f0xx_hal.h"

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

/*
 * Time quanta configuration
 */
#define CAN_SJW     CAN_SJW_1TQ
#define CAN_BS1     CAN_BS1_4TQ
#define CAN_BS2     CAN_BS2_3TQ

/**
 * List of prescalers for the CAN peripheral clock
 * in order to achieve the corresponding bitrates
 * with the above time quanta
 */
enum can_prescaler {
    CAN_PRESCALER_10K = 600,
    CAN_PRESCALER_20K = 300,
    CAN_PRESCALER_50K = 120,
    CAN_PRESCALER_100K = 60,
    CAN_PRESCALER_125K = 48,
    CAN_PRESCALER_250K = 24,
    CAN_PRESCALER_500K = 12,
    CAN_PRESCALER_750K = 8,
    CAN_PRESCALER_1000K = 6,
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
 * Set the CAN operating mode to silent, i.e. disable transmissions
 */
void can_set_silent(uint8_t silent);

/**
 * Transmit a CAN frame in blocking mode
 */
uint32_t can_tx(CanTxMsgTypeDef *tx_msg, uint32_t timeout);

/**
 * Retrieve a received CAN frame from the FIFO
 */
uint32_t can_rx(CanRxMsgTypeDef *rx_msg, uint32_t timeout);

/**
 * Check, whether a CAN frame was received
 */
uint8_t is_can_msg_pending(uint8_t fifo);

/**
 * Enable monitoring the CAN bus chatter for frames with a specific CAN ID or mask
 */
void can_set_filter(uint32_t id, uint32_t mask);

#endif // _CAN_H
