/**
* @file  main.c
* @brief Main program body
*/

#include "stm32f0xx_hal.h"
#include "usb_device.h"

#include "clock.h"
#include "can.h"
#include "led.h"


int main()
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    can_init();
    led_init();
    MX_USB_DEVICE_Init();

    // Loop forever
    for (;;)
    {
        can_process();
        led_process();
    }
}
