/**
 * @file
 * This file implements functions to read out the
 * hard-coded microcontroller (MCU) die identification codes.
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

#include <uid.h>


/**
 * Device electronic signature,
 * specifically the unique device ID registers
 * see RM0091, p.925
 */
static const uint32_t* STM32_UID = ((uint32_t*) 0x1FFFF7AC);


static inline uint16_t bcd2int(uint16_t a)
{
    return    (a & 0x000F) +
            (((a & 0x00F0) >> 4) * 10) +
            (((a & 0x0F00) >> 8) * 100) +
            (((a & 0xF000) >> 12) * 1000);
}


uint16_t uid_get_wafer_x()
{
    return bcd2int(STM32_UID[0] >> 16);
}


uint16_t uid_get_wafer_y()
{
    return bcd2int(STM32_UID[0] & 0x0000FFFF);
}


uint8_t uid_get_wafer_number()
{
    return STM32_UID[1] & 0x000000FF;
}


char* uid_get_lot_number()
{
    static uint8_t s[8];
    s[0] =  STM32_UID[1] >> 24;
    s[1] = (STM32_UID[1] >> 16) & 0xFF;
    s[2] = (STM32_UID[1] >>  8) & 0xFF;
    s[3] =  STM32_UID[2] >> 24;
    s[4] = (STM32_UID[2] >> 16) & 0xFF;
    s[5] = (STM32_UID[2] >>  8) & 0xFF;
    s[6] =  STM32_UID[2] & 0xFF;
    s[7] = '\0';
    return (char*) s;
}
