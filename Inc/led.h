/**
 * @file
 * @brief Header file for LED control implemented in @ref led.c
 */

#ifndef _LED_H
#define _LED_H

#include "stm32f0xx_hal.h"
#include "platform.h"
#include "config.h"

typedef enum
{
    LED_POWER,
    LED_ACTIVITY,
    LED_ERROR,
} led_index_t;

#define LED_POWER_PORT      LED_GREEN_PORT
#define LED_POWER_PIN       LED_GREEN_PIN
#define LED_ACTIVITY_PORT   LED_GREEN_PORT
#define LED_ACTIVITY_PIN    LED_GREEN_PIN
#define LED_ERROR_PORT      LED_RED_PORT
#define LED_ERROR_PIN       LED_RED_PIN

#define LED_POWER_ENABLED
#define LED_ACTIVITY_ENABLED
#if PLATFORM == CANTACT
#define LED_ERROR_ENABLED
#endif


/**
 * Initialize the GPIOs which connect to LEDs
 */
void led_init();

/**
 * Turns the LED on
 */
void led_on(led_index_t);

/**
 * Force LED off immediately
 */
void led_off(led_index_t);

/**
 * Switches the LED off after @ref LED_ON_DURATION iterations
 */
void led_process();


#endif
