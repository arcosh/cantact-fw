
#include "platform.h"

#ifdef PLATFORM_NUCLEO

#include "config.h"
#include "usart.h"
#include "slcan.h"
#include "fifo.h"
#include <stm32f0xx_hal.h>


UART_HandleTypeDef husart2;

uint8_t uart_rx_buffer[UART_RX_BUFFER_SIZE];
fifo_t uart_rx_fifo;

uint8_t uart_tx_buffer[UART_TX_BUFFER_SIZE];
fifo_t uart_tx_fifo;

volatile uint8_t slcan_command_count = 0;


void uart_init()
{
    // Initialize FIFO buffers
    fifo_init(&uart_rx_fifo, uart_rx_buffer, sizeof(uart_rx_buffer));
    fifo_init(&uart_tx_fifo, uart_tx_buffer, sizeof(uart_tx_buffer));

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
    husart2.Instance = UART_PERIPHERAL;
    husart2.Init.BaudRate = UART_BAUDRATE;
    husart2.Init.WordLength = UART_WORDLENGTH_8B;
    husart2.Init.StopBits = UART_STOPBITS_1;
    husart2.Init.Parity = UART_PARITY_NONE;
    husart2.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&husart2);

    HAL_NVIC_SetPriority(UART_IRQ, IRQ_PRIORITY_UART, 0);
    HAL_NVIC_EnableIRQ(UART_IRQ);

//    HAL_UART_Receive_IT(&husart2, slcan_str, 1);

    // Enable receiver has data interrupt
    __HAL_UART_ENABLE_IT(&husart2, UART_IT_RXNE);
}


void USART2_IRQHandler()
{
    // This is apparently necessary, otherwise the USART transmitter will forever stop transmitting upon data reception.
    husart2.Instance->ICR = ~0;

//    (husart2.Instance->CR1 & USART_CR1_RXNEIE) > 0
    if ((husart2.Instance->ISR & USART_ISR_RXNE) > 0)
    {
        // Pop byte from UART to FIFO
        uint8_t rx_byte[2];
        rx_byte[0] = husart2.Instance->RDR & 0xFF;
        __HAL_UART_SEND_REQ(&husart2, UART_RXDATA_FLUSH_REQUEST);

        // Is there room for one more byte in the FIFO ?
        enter_critical();
        if (fifo_has_room(&uart_rx_fifo, 1))
        {
            fifo_push(&uart_rx_fifo, rx_byte, 1);

            if (rx_byte[0] == SLCAN_COMMAND_TERMINATOR)
                slcan_command_count++;
        }
        exit_critical();
    }

    if (((husart2.Instance->CR1 & USART_CR1_TXEIE) > 0)
     && ((husart2.Instance->ISR & USART_ISR_TXE) > 0))
    {
        enter_critical();
        if (fifo_is_empty(&uart_tx_fifo))
        {
            // Disable transmission buffer ready interrupt
            __HAL_UART_DISABLE_IT(&husart2, UART_IT_TXE);
        }
        else
        {
            // Push another byte to the UART transmission buffer
            uint8_t tx_byte[2];
            fifo_pop(&uart_tx_fifo, tx_byte, 1);
            husart2.Instance->TDR = tx_byte[0];
        }
        exit_critical();
    }
}


void uart_process()
{
    enter_critical();
    if (slcan_command_count == 0)
    {
        exit_critical();
        return;
    }

    // Did we receive a complete SLCAN command yet?
    uint16_t slcan_command_length;
    if (fifo_has_slcan_command(&uart_rx_fifo, &slcan_command_length))
    {
        // Pop the command from the FIFO and perform initial SLCAN parsing!
        uint8_t slcan_command[SLCAN_MTU+1];
        fifo_pop(&uart_rx_fifo, slcan_command, slcan_command_length);
        slcan_command_count--;
        exit_critical();
        slcan_parse_command(slcan_command, slcan_command_length);
    }
    else
    {
        exit_critical();
    }
}


int _read(int file, char *ptr, int len)
{
//    HAL_UART_Receive(&husart2, (uint8_t*) ptr, 1, 300);
//    return 1;
    return 0;
}


int _write(int file, char *ptr, int len)
{
    // Append data to USART transmission buffer
    enter_critical();
    fifo_push(&uart_tx_fifo, (uint8_t*) ptr, len);
    exit_critical();

    // Enable transmitter buffer ready interrupt
    __HAL_UART_ENABLE_IT(&husart2, UART_IT_TXE);

    // Assume transmission success
    return len;
}

#endif
