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

    // turn on green LED
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

    // blink red LED for test
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

    // loop forever
    CanRxMsgTypeDef rx_msg;
    uint32_t status;
    uint8_t msg_buf[SLCAN_MTU];


    for (;;) {
		while (!is_can_msg_pending(CAN_FIFO0))
			led_process();
		status = can_rx(&rx_msg, 3);
		if (status == HAL_OK) {
			status = slcan_parse_frame((uint8_t *)&msg_buf, &rx_msg);
			CDC_Transmit_FS(msg_buf, status);
		}
		led_process();
    }
}
