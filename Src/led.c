/**
 * @file
 * @brief Library to handle LED blinking
 */

#include "led.h"

/**
 * Tick count, when activity LED was last switched on
 */
static uint32_t activity_led_last_on = 0;

/**
 * Tick count, when activity LED was last switched off
 */
static uint32_t activity_led_lastoff = 0;


void led_init()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
    GPIO_InitStruct.Alternate = 0;

    #ifdef LED_POWER_ENABLED
    GPIO_InitStruct.Pin = LED_POWER_PIN;
    HAL_GPIO_Init(LED_POWER_PORT, &GPIO_InitStruct);
    #endif

    #ifdef LED_ACTIVITY_ENABLED
    GPIO_InitStruct.Pin = LED_ACTIVITY_PIN;
    HAL_GPIO_Init(LED_ACTIVITY_PORT, &GPIO_InitStruct);
    #endif

    #ifdef LED_ERROR_ENABLED
    GPIO_InitStruct.Pin = LED_ERROR_PIN;
    HAL_GPIO_Init(LED_ERROR_PORT, &GPIO_InitStruct);
    #endif
}


void led_on(led_index_t led)
{
    if (led == LED_POWER)
    {
        HAL_GPIO_WritePin(LED_POWER_PORT, LED_POWER_PIN, GPIO_PIN_SET);
        return;
    }

	// Make sure the LED has been off for at least LED_DURATION before turning on again
	// This prevents a solid status LED on a busy canbus
	if ((led == LED_ACTIVITY)
	 && (activity_led_last_on == 0 && HAL_GetTick() - activity_led_lastoff > LED_ON_DURATION))
	{
		HAL_GPIO_WritePin(LED_ACTIVITY_PORT, LED_ACTIVITY_PIN, GPIO_PIN_SET);
		activity_led_last_on = HAL_GetTick();
	}
}


void led_off(led_index_t led)
{
    if (led == LED_POWER)
    {
        HAL_GPIO_WritePin(LED_POWER_PORT, LED_POWER_PIN, GPIO_PIN_RESET);
        return;
    }
    if (led == LED_ACTIVITY)
    {
        HAL_GPIO_WritePin(LED_ACTIVITY_PORT, LED_ACTIVITY_PIN, GPIO_PIN_RESET);
    }
}


void led_process()
{
	// If LED has been on for long enough, turn it off
	if(activity_led_last_on > 0 && HAL_GetTick() - activity_led_last_on > LED_ON_DURATION)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
		activity_led_last_on = 0;
		activity_led_lastoff = HAL_GetTick();
	}
}
