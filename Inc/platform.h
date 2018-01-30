/**
 * @file platform.h
 *
 * This file defines hardware-related constants.
 */

#ifndef PLATFORM_H
#define PLATFORM_H

#include <stm32f0xx_hal.h>

#ifdef PLATFORM_CANTACT
#define EXTERNAL_OSCILLATOR
#define HSE_VALUE       16000000
#endif

#ifdef PLATFORM_NUCLEO
#define INTERNAL_OSCILLATOR
#endif

#ifdef PLATFORM_CANTACT
#define PC_INTERFACE_USB
#endif

#ifdef PLATFORM_NUCLEO
#define PC_INTERFACE_UART
#define UART_PERIPHERAL USART2
#define UART_IRQ        USART2_IRQn
#define UART_GPIO_AF    GPIO_AF1_USART2
#define UART_RX_PORT    GPIOA
#define UART_RX_PIN     GPIO_PIN_15
#define UART_TX_PORT    GPIOA
#define UART_TX_PIN     GPIO_PIN_2
#endif

#ifdef PLATFORM_CANTACT
#define LED_GREEN_PORT  GPIOB
#define LED_GREEN_PIN   GPIO_PIN_0
#define LED_RED_PORT    GPIOB
#define LED_RED_PIN     GPIO_PIN_1
#endif

#ifdef PLATFORM_NUCLEO
#define LED_GREEN_PORT  GPIOB
#define LED_GREEN_PIN   GPIO_PIN_3
#endif

#define CAN_PERIPHERAL  CAN

#ifdef PLATFORM_CANTACT
#define CAN_RX_PORT     GPIOB
#define CAN_RX_PIN      GPIO_PIN_8
#define CAN_TX_PORT     GPIOB
#define CAN_TX_PIN      GPIO_PIN_9
#endif

#ifdef PLATFORM_NUCLEO
#define CAN_RX_PORT     GPIOA
#define CAN_RX_PIN      GPIO_PIN_11
#define CAN_TX_PORT     GPIOA
#define CAN_TX_PIN      GPIO_PIN_12
#endif

#define enter_critical()    do { asm("CPSID  i"); __DSB(); __ISB(); } while (0);
#define exit_critical()     asm("CPSIE  i");

#endif
