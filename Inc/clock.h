
#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

/**
 * Configure the system clock
 */
void SystemClock_Config();

/**
 * Enable clock to GPIO ports
 */
void MX_GPIO_Init();

/**
 * Returns number of milliseconds since startup
 */
uint32_t get_time();

#endif
