
#include "platform.h"

#if PLATFORM == NUCLEO

#include "usart.h"
#include <stm32f0xx_hal.h>


UART_HandleTypeDef husart2;


void uart_init()
{
    // Enable GPIO clock
    __GPIOA_CLK_ENABLE();

    // Configure GPIO pins as USART pins
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = UART_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = UART_GPIO_AF;
    HAL_GPIO_Init(UART_TX_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = UART_RX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = UART_GPIO_AF;
    HAL_GPIO_Init(UART_RX_PORT, &GPIO_InitStruct);

    // Enable USART clock
    __USART2_CLK_ENABLE();

    // Initialize USART2 as UART interface
    husart2.Instance = UART_INSTANCE;
    husart2.Init.BaudRate = 460800;
    husart2.Init.WordLength = UART_WORDLENGTH_8B;
    husart2.Init.StopBits = UART_STOPBITS_1;
    husart2.Init.Parity = UART_PARITY_NONE;
    husart2.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&husart2);
}


int _read(int file, char *ptr, int len)
{
    HAL_UART_Receive(&husart2, (uint8_t*) ptr, 1, 300);
    return 1;
}


int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&husart2, (uint8_t*) ptr, (uint16_t) len, 300);
    return len;
}

#endif
