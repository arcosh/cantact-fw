/**
 * @file
 * @brief Library to interpret and generate serial CAN (SLCAN) messages
 */

#include "stm32f0xx_hal.h"
#include "can.h"
#include "fifo.h"
#include "slcan.h"
#include <error.h>

static uint32_t current_filter_id = 0;
static uint32_t current_filter_mask = 0;


int8_t slcan_parse_frame(CanRxMsgTypeDef* frame, uint8_t* buf) {
    uint8_t i = 0;
    uint8_t id_len, j;
    uint32_t tmp;

    for (j=0; j < SLCAN_MTU; j++) {
        buf[j] = '\0';
    }

    // add character for frame type
    if (frame->RTR == CAN_RTR_DATA) {
        buf[i] = 't';
    } else if (frame->RTR == CAN_RTR_REMOTE) {
        buf[i] = 'r';
    }

    // assume standard identifier
    id_len = SLCAN_STD_ID_LEN;
    tmp = frame->StdId;
    // check if extended
    if (frame->IDE == CAN_ID_EXT) {
        // convert first char to upper case for extended frame
        buf[i] -= 32;
        id_len = SLCAN_EXT_ID_LEN;
        tmp = frame->ExtId;
    }
    i++;

    // add identifier to buffer
    for(j=id_len; j > 0; j--) {
        // add nibble to buffer
        buf[j] = (tmp & 0xF);
        tmp = tmp >> 4;
        i++;
    }

    // add DLC to buffer
    buf[i++] = frame->DLC;

    // add data bytes
    for (j = 0; j < frame->DLC; j++) {
        buf[i++] = (frame->Data[j] >> 4);
        buf[i++] = (frame->Data[j] & 0x0F);
    }

    // convert to ASCII (2nd character to end)
    for (j = 1; j < i; j++) {
        if (buf[j] < 0xA) {
            buf[j] += 0x30;
        } else {
            buf[j] += 0x37;
        }
    }

    // add carriage return (slcan EOL)
    buf[i++] = SLCAN_COMMAND_TERMINATOR;

    // return number of bytes in string
    return i;
}


inline uint8_t hex2int(uint8_t c) {
    if (c >= 'a' && c <= 'f') {
        // Lowercase letters
        c = c - 'a' + 10;
    }
    else if (c >= 'A' && c <= 'F') {
        // Uppercase letters
        c = c - 'A' + 10;
    }
    else if (c >= '0' && c <= '9') {
        // Digits
        c = c - '0';
    }
    return c;
}


int8_t slcan_parse_command(uint8_t* buf, uint8_t len) {

    /*
     * Evaluate first byte in order to determine command type
     */
    if (buf[0] == SLCAN_OPEN_CHANNEL) {
        current_filter_id = 0;
        current_filter_mask = 0;
        can_enable();
        return SUCCESS;

    } else if (buf[0] == SLCAN_CLOSE_CHANNEL) {
        can_disable();
        return SUCCESS;

    } else if (buf[0] == SLCAN_SET_BITRATE_CANONICAL) {
        // set bitrate command
        switch(buf[1]) {
        case 0:
            can_set_bitrate(CAN_BITRATE_10K);
            break;
        case 1:
            can_set_bitrate(CAN_BITRATE_20K);
            break;
        case 2:
            can_set_bitrate(CAN_BITRATE_50K);
            break;
        case 3:
            can_set_bitrate(CAN_BITRATE_100K);
            break;
        case 4:
            can_set_bitrate(CAN_BITRATE_125K);
            break;
        case 5:
            can_set_bitrate(CAN_BITRATE_250K);
            break;
        case 6:
            can_set_bitrate(CAN_BITRATE_500K);
            break;
        case 7:
            can_set_bitrate(CAN_BITRATE_750K);
            break;
        case 8:
            can_set_bitrate(CAN_BITRATE_1000K);
            break;
        default:
            return ERROR_SLCAN_INVALID_BITRATE;
        }
        return SUCCESS;

    } else if (buf[0] == SLCAN_SET_ACCEPTANCE_CODE) {

        // must have one exactly 8 bytes long argument
        if (len != 9)
            return ERROR_SLCAN_INVALID_ARGUMENT;

        // set filter command
        uint32_t id = 0;
        for (uint8_t i=1; i <= 8; i++) {
            id <<= 4;
            id += hex2int(buf[i]);
        }
        current_filter_id = id;
        can_set_filter(current_filter_id, current_filter_mask);
        return SUCCESS;

    } else if (buf[0] == SLCAN_SET_ACCEPTANCE_MASK) {

        // must have one exactly 8 bytes long argument
        if (len != 9)
            return ERROR_SLCAN_INVALID_ARGUMENT;

        // set mask command
        uint32_t mask = 0;
        for (uint8_t i=1; i <= 8; i++) {
            mask <<= 4;
            mask += hex2int(buf[i]);
        }
        current_filter_mask = mask;
        can_set_filter(current_filter_id, current_filter_mask);
        return SUCCESS;

    } else if ((buf[0] == SLCAN_TRANSMIT_STANDARD)
            || (buf[0] == SLCAN_TRANSMIT_EXTENDED)
            || (buf[0] == SLCAN_TRANSMIT_REQUEST_STANDARD)
            || (buf[0] == SLCAN_TRANSMIT_REQUEST_EXTENDED)) {
        extern fifo_t can_tx_fifo;
        if (fifo_push(&can_tx_fifo, buf, len))
            // ok
            return SUCCESS;
        // error
        return ERROR_TX_FIFO_OVERRUN;
    }

    return ERROR_SLCAN_COMMAND_NOT_RECOGNIZED;
}


bool slcan_parse_transmit_command(uint8_t* buffer, uint16_t length, CanTxMsgTypeDef* frame) {

    if (length == 0)
        // Empty buffer
        return false;

    if ((buffer[0] != SLCAN_TRANSMIT_STANDARD)
     && (buffer[0] != SLCAN_TRANSMIT_EXTENDED)
     && (buffer[0] != SLCAN_TRANSMIT_REQUEST_STANDARD)
     && (buffer[0] != SLCAN_TRANSMIT_REQUEST_EXTENDED))
        // Invalid transmit command
        return false;

    // Parser position in the buffer
    uint8_t i = 0;

    frame->IDE = ((buffer[i] == SLCAN_TRANSMIT_STANDARD) || (buffer[i] == SLCAN_TRANSMIT_REQUEST_STANDARD))
                    ? CAN_ID_STD
                    : CAN_ID_EXT;

    frame->RTR = ((buffer[i] == SLCAN_TRANSMIT_REQUEST_STANDARD) || (buffer[i] == SLCAN_TRANSMIT_REQUEST_EXTENDED))
                    ? CAN_RTR_REMOTE
                    : CAN_RTR_DATA;

    frame->StdId = 0;
    frame->ExtId = 0;
    if (frame->IDE == CAN_ID_STD) {
        // Parse hexadecimal representation of standard CAN ID
        for (uint8_t j=i; j <= SLCAN_STD_ID_LEN; j++, i++) {
            frame->StdId <<= 4;
            frame->StdId += hex2int(buffer[j]);
        }
    } else {
        // Parse hexadecimal representation of extended CAN ID
        for (uint8_t j=i; j <= SLCAN_EXT_ID_LEN; j++, i++) {
            frame->ExtId <<= 4;
            frame->ExtId += hex2int(buffer[j]);
        }
    }

    frame->DLC = hex2int(buffer[i++]);
    if (frame->DLC < 0 || frame->DLC > 8) {
        return false;
    }

    if (i + 1 + frame->DLC*2 > length)
        return false;

    // Parse data from hexadecimal representation
    for (uint8_t j=0; j < frame->DLC; j++, i+=2) {
        frame->Data[j] = (hex2int(buffer[i]) << 4);
        frame->Data[j] += hex2int(buffer[i+1]);
    }

    return true;
}
