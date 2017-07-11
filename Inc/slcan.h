/**
 * @file
 * @brief Header file for the SLCAN library implemented in @ref slcan.c
 */

#ifndef _SLCAN_H
#define _SLCAN_H

/**
 * The maximum transfer unit (MTU),
 * i.e. the maximum number of bytes possible in one SLCAN message,
 * is the length of an extended CAN frame with timestamp:
 *
 *  sizeof("T1111222281122334455667788EA5F\r")+1
 *
 * See also the can-utils project -> slcanpty.c
 */
#define SLCAN_MTU 30

/** Length of the standard CAN ID */
#define SLCAN_STD_ID_LEN 3
/** Length of the extended CAN ID */
#define SLCAN_EXT_ID_LEN 8

/**
 * @brief  Parses CAN frame and generates SLCAN message
 * @param  buf:   Pointer to SLCAN message buffer
 * @param  frame: Pointer to CAN frame received from CAN interface
 * @return Number of bytes in generated SLCAN message
 */
int8_t slcan_parse_frame(uint8_t *buf, CanRxMsgTypeDef *frame);


/**
 * @brief  Parses SLCAN message and configures CAN peripheral accordingly or transmits CAN frame
 * @param  buf: Pointer to SLCAN message
 * @param  len: Number of bytes in SLCAN message
 * @return Zero if successful, other values indicate an error
 */
int8_t slcan_parse_str(uint8_t *buf, uint8_t len);

#endif // _SLCAN_H
