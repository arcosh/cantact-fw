/**
* @file  main.c
* @brief Main program body
*/

#include "stm32f0xx_hal.h"
#include "platform.h"
#include "clock.h"
#include "can.h"
#include "led.h"

#include "usb_device.h"
#include "usart.h"

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
    #if PC_INTERFACE == PC_IF_USB
    MX_USB_DEVICE_Init();
    #endif
    #if PC_INTERFACE == PC_IF_UART
    uart_init();
    #endif

    // Loop forever
    for (;;)
    {
        can_process();
        led_process();
    }
}
