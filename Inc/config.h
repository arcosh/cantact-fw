/**
 * @file config.h
 *
 * This file configures variable runtime parameters.
 */

#ifndef CONFIG_H
#define CONFIG_H

#define IRQ_PRIORITY_SYSTICK    1
#define IRQ_PRIORITY_UART       2
#define IRQ_PRIORITY_USB        2
#define IRQ_PRIORITY_CAN        0

#ifdef PLATFORM_NUCLEO
#define UART_BAUDRATE               460800
#define CAN_RX_FRAME_BUFFER_SIZE    750
#define CAN_TX_FRAME_BUFFER_SIZE    750
#define CAN_RX_SERIAL_BUFFER_SIZE   750
#define CAN_TX_SERIAL_BUFFER_SIZE   750
#endif

#ifdef PLATFORM_CANTACT
#define CAN_RX_FRAME_BUFFER_SIZE    20
#define CAN_TX_FRAME_BUFFER_SIZE    20
#define CAN_RX_SERIAL_BUFFER_SIZE   200
#define CAN_TX_SERIAL_BUFFER_SIZE   200
#endif

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
