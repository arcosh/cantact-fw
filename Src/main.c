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
    #ifdef PC_INTERFACE_USB
    MX_USB_DEVICE_Init();
    #endif
    #ifdef PC_INTERFACE_UART
    uart_init();
    #endif

    // Loop forever
    for (;;)
    {
        process_can();
        process_led();
    }
}
