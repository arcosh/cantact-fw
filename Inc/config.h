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
#define IRQ_PRIORITY_USB        2
#define IRQ_PRIORITY_CAN        1

#define CAN_RX_BUFFER_SIZE      370
#define CAN_TX_BUFFER_SIZE      370

#endif
