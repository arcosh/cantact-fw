/**
 * @file error.h
 *
 * Declaration of the possible error return values
 */

#ifndef ERROR_H
#define ERROR_H

#define SUCCESS                             0
#define ERROR_SLCAN_COMMAND_NOT_RECOGNIZED  10
#define ERROR_SLCAN_COMMAND_NOT_SUPPORTED   11
#define ERROR_SLCAN_INVALID_ARGUMENT        12
#define ERROR_SLCAN_INVALID_BITRATE         13
#define ERROR_TX_FIFO_OVERRUN               20

#endif // ERROR_H
