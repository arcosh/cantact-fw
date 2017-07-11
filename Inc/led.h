/**
 * @file
 * @brief Header file for LED control implemented in @ref led.c
 */

#ifndef _LED_H
#define _LED_H

/**
 * Number of @ref led_process iterations the LED should stay on
 */
#define LED_DURATION 50

/**
 * @brief Turns the LED on
 */
void led_on(void);

/**
 * @brief Handler, which switches the LED off after @ref LED_DURATION iterations
 */
void led_process(void);

#endif
