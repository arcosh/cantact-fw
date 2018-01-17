/**
 * @file platform.h
 *
 * This file defines hardware-related constants.
 */

#ifndef PLATFORM_H
#define PLATFORM_H

#define LED_GREEN_PORT  GPIOB
#define LED_GREEN_PIN   GPIO_PIN_0
#define LED_RED_PORT    GPIOB
#define LED_RED_PIN     GPIO_PIN_1

#define CAN_PERIPHERAL  CAN
#define CAN_RX_PORT     GPIOB
#define CAN_RX_PIN      GPIO_PIN_8
#define CAN_TX_PORT     GPIOB
#define CAN_TX_PIN      GPIO_PIN_9

#define enter_critical()    do { asm("CPSID  i"); __DSB(); __ISB(); } while (0);
#define exit_critical()     asm("CPSIE  i");

#endif
