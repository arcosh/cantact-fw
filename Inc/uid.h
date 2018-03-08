/**
 * @file
 * This file declares functions to read out the
 * hard-coded microcontroller (MCU) die identification codes.
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

#ifndef UID_H
#define UID_H

#include <stdint.h>

/**
 * Returns a pointer to the 8 char long string
 * representing the production lot ID
 */
char* uid_get_lot_number();

/**
 * Returns the number of the wafer,
 * this MCU was manufactured on,
 * within the above production lot
 */
uint8_t uid_get_wafer_number();

/**
 * Returns the X coordinate of this MCU's die
 * on the wafer, it was produced on
 */
uint16_t uid_get_wafer_x();

/**
 * Returns the Y coordinate of this MCU's die
 * on the wafer, it was produced on
 */
uint16_t uid_get_wafer_y();

#endif
