/**
 * @file
 * @brief Header file for the SLCAN library implemented in @ref slcan.c
 */

#ifndef _SLCAN_H
#define _SLCAN_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx_hal.h"


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

#define SLCAN_COMMAND_TERMINATOR    '\r'

/**
 * Serial CAN message types
 *
 * The SLCAN protocol was specified by Lawicel AB, Sweden,
 * for their CAN232 and CANUSB adapters.
 * Several products exist on the market,
 * which use the protocol and add their own extensions,
 * including CANtact, USBTIN and Mictronics USB-CAN.
 *
 * @attention
 * The following specification collisions exist:
 * 'A': between Lawicel CAN232 and Mictronics CAN-USB
 * 'm': between Lawicel CAN232 and CANtact
 * 'M': between Lawicel CAN232 and CANtact
 */
enum slcan_message_type {
    SLCAN_OPEN_CHANNEL = 'O',
    SLCAN_CLOSE_CHANNEL = 'C',

    SLCAN_GET_HARDWARE_VERSION = 'V',
    SLCAN_GET_FIRMWARE_VERSION = 'v',
    SLCAN_GET_SERIAL_NUMBER = 'N',

    SLCAN_SET_BITRATE_CANONICAL = 'S',
    SLCAN_SET_BITRATE_ARBITRARY = 's',

    SLCAN_GET_STATUS = 'F',
    SLCAN_SET_TIMESTAMPING = 'Z',

    SLCAN_TRANSMIT_STANDARD = 't',
    SLCAN_TRANSMIT_EXTENDED = 'T',
    SLCAN_TRANSMIT_REQUEST_STANDARD = 'r',
    SLCAN_TRANSMIT_REQUEST_EXTENDED = 'R',

    SLCAN_SET_UART_BAUDRATE = 'U',
    SLCAN_FIFO_POLL_SINGLE = 'P',
    SLCAN_FIFO_POLL_ALL = 'A',
    SLCAN_SET_AUTOPOLL = 'X',

    SLCAN_SET_FILTER_MODE = 'W',
    SLCAN_SET_ACCEPTANCE_MASK = 'm',
    SLCAN_SET_ACCEPTANCE_CODE = 'M',
    SLCAN_SET_AUTOSTARTUP = 'Q',

    CANTACT_SET_MODE1 = 'm',
    CANTACT_SET_MODE2 = 'M',
    CANTACT_SET_FILTER = 'F',
    CANTACT_SET_MASK = 'K',
    CANTACT_START_BOOTLOADER = 'B',

    USBTIN_OPEN_LOOPBACK = 'I',
    USBTIN_OPEN_LISTEN_ONLY = 'L',
    USBTIN_READ_REGISTER = 'G',
    USBTIN_WRITE_REGISTER = 'W',

    MICTRONICS_GET_ERROR = 'E',
};


/**
 * @brief  Parses CAN frame and generates SLCAN message
 * @param  buf:   Pointer to SLCAN message buffer
 * @param  frame: Pointer to CAN frame received from CAN interface
 * @return Number of bytes in generated SLCAN message
 */
int8_t slcan_parse_frame(CanRxMsgTypeDef* frame, uint8_t* buf);


/**
 * @brief  Parses SLCAN message and configures CAN peripheral accordingly or transmits CAN frame
 * @param  buf: Pointer to SLCAN message
 * @param  len: Number of bytes in SLCAN message
 * @return Zero if successful, other values indicate an error, see \ref error.h
 */
int8_t slcan_parse_command(uint8_t *buf, uint8_t len);


/**
 * Generates a CAN frame according to an SLCAN transmit command
 *
 * @param buffer    Pointer to SLCAN string
 * @param length    Length of SLCAN string
 * @param frame     Pointer to CAN frame structure to configure according to SLCAN string
 * @return true     Parser success
 * @return false    Failed to configure frame according to SLCAN string
 */
bool slcan_parse_transmit_command(uint8_t* buffer, uint16_t length, CanTxMsgTypeDef* frame);


#endif // _SLCAN_H
