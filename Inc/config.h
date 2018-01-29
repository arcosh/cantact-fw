/**
 * @file config.h
 *
 * This file configures variable runtime parameters.
 */

#ifndef CONFIG_H
#define CONFIG_H

//#define INTERNAL_OSCILLATOR
#define EXTERNAL_OSCILLATOR

#define IRQ_PRIORITY_SYSTICK    3
#define IRQ_PRIORITY_UART       2
#define IRQ_PRIORITY_USB        2
#define IRQ_PRIORITY_CAN        1

#define UART_BAUDRATE           460800

#define CAN_RX_BUFFER_SIZE      370
#define CAN_TX_BUFFER_SIZE      370

#define CAN_TX_TIMEOUT          20

#define LED_POWER_ENABLED
#define LED_ACTIVITY_ENABLED
#ifdef PLATFORM_CANTACT
#define LED_ERROR_ENABLED
#endif

/**
 * Number of @ref led_process iterations the LED should stay on
 */
#define LED_ON_DURATION         250

#endif
