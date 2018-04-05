/**
 * @file
 * @brief This file implements functions to configure and use the Controller Area Network (CAN) bus
 */

#include "can.h"

#include "platform.h"
#include "config.h"

#include "slcan.h"
#include "led.h"
#include "fifo.h"
#include "buffering.h"

#include "usbd_cdc_if.h"
#include "usart.h"


/**
 * Handle to access the MCU's CAN peripheral
 */
CAN_HandleTypeDef hcan;

/**
 * Clock prescaler for CAN peripheral
 */
static uint32_t prescaler;

/**
 * Current state of the CAN peripheral (on or off)
 */
enum can_bus_state bus_state;


void can_init(void)
{
    // Default speed: 1 Mbps
    hcan.Instance = CAN_PERIPHERAL;
    prescaler = CAN_PRESCALER_1000K;
    // Reset bxCAN peripheral
    can_disable();

    buffering_init();
}


void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
  if(hcan->Instance == CAN_PERIPHERAL)
  {
    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __CAN_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_CAN;
    GPIO_InitStruct.Pin = CAN_RX_PIN;
    HAL_GPIO_Init(CAN_RX_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_CAN;
    GPIO_InitStruct.Pin = CAN_TX_PIN;
    HAL_GPIO_Init(CAN_TX_PORT, &GPIO_InitStruct);
  }
}


void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hcan)
{
  if(hcan->Instance == CAN_PERIPHERAL)
  {
    __CAN_CLK_DISABLE();

    HAL_GPIO_DeInit(CAN_RX_PORT, CAN_RX_PIN);
    HAL_GPIO_DeInit(CAN_TX_PORT, CAN_TX_PIN);
  }
}


void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
    // Copy frame to simple type
    can_frame_t frame;
    frame.dlc = hcan->pRxMsg->DLC;
    // TODO: Support ExtID and RTR
    frame.id = hcan->pRxMsg->StdId;
    frame.is_extended_id = false;
    frame.is_rtr = false;
    for (uint8_t i=0; i<frame.dlc; i++)
    {
        frame.data[i] = hcan->pRxMsg->Data[i];
    }

    // Push one frame to the RX frame buffer
    fifo_push(&can_rx_frame_fifo, (uint8_t*) (&frame), sizeof(can_frame_t));

    // Receive more frames
//    HAL_CAN_Receive_IT(hcan, CAN_FIFO0);
    __HAL_CAN_ENABLE_IT(hcan, CAN_IT_FMP0);
}


//void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef *hcan)
//{
//    // Remove last transmitted frame from buffer
//    fifo_drop_oldest_entry(&can_tx_fifo);
//}


void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
    // TODO: Resolve errors
    led_on(LED_ERROR);

    // Reset bxCAN
    hcan->Instance->MCR |= CAN_MCR_RESET | CAN_MCR_SLEEP;
    // Wait until sleep mode is reached
    while ((hcan->Instance->MSR & CAN_MSR_SLAK) == 0);
    // Request initialization
    hcan->Instance->MCR &= ~(CAN_MCR_INRQ | CAN_MCR_SLEEP);
    // Wait for confirmation of switch to normal mode
    while ((hcan->Instance->MSR & CAN_MSR_INAK) > 0);

    /* Enable Error warning Interrupt */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_EWG);

    /* Enable Error passive Interrupt */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_EPV);

    /* Enable Bus-off Interrupt */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_BOF);

    /* Enable Last error code Interrupt */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_LEC);

    /* Enable Error Interrupt */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_ERR);
}


inline void can_enable_interrupt_switches() {
    /*
     * FMP0: FIFO0 message pending interrupt switch
     * TME: Transmit mailbox empty
     */
    __HAL_CAN_ENABLE_IT(&hcan, CAN_IT_FMP0);
}


void CEC_CAN_IRQHandler()
{
    HAL_CAN_IRQHandler(&hcan);

    // Re-enable interrupts after the HAL IRQ handler disables them
    can_enable_interrupt_switches();
}


void can_enable(void) {
    if (bus_state == ON_BUS) {
        can_disable();
    }

    // Clear FIFO buffers
    fifo_init(&can_rx_fifo, can_rx_buffer, CAN_RX_BUFFER_SIZE);
    fifo_init(&can_tx_fifo, can_tx_buffer, CAN_TX_BUFFER_SIZE);

    hcan.pRxMsg = &can_rx_frame;
    hcan.pTxMsg = 0;

    hcan.Init.Prescaler = prescaler;
    hcan.Init.Mode = CAN_MODE_NORMAL;
    hcan.Init.SJW = CAN_SJW;
    hcan.Init.BS1 = CAN_BS1;
    hcan.Init.BS2 = CAN_BS2;
    hcan.Init.TTCM = DISABLE;
    hcan.Init.ABOM = ENABLE;
    hcan.Init.AWUM = ENABLE;
    hcan.Init.NART = DISABLE;
    hcan.Init.RFLM = ENABLE;
    hcan.Init.TXFP = 1;
    if (HAL_CAN_Init(&hcan) == HAL_OK)
    {
        led_on(LED_ACTIVITY);
        led_off(LED_ERROR);
    }
    else
    {
        led_on(LED_ERROR);
    }
    bus_state = ON_BUS;

    // No filtering: Receive all frames
    can_set_filter(0, 0);

    // Enable interrupts
    HAL_NVIC_SetPriority(CEC_CAN_IRQn, IRQ_PRIORITY_CAN, 0);
    HAL_NVIC_EnableIRQ(CEC_CAN_IRQn);

//    HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
//    __HAL_CAN_ENABLE_IT(&hcan, CAN_IT_FMP0);
    hcan.Instance->IER = CAN_IT_FMP0;
}


void can_disable(void) {
    // Disable interrupts
    HAL_NVIC_DisableIRQ(CEC_CAN_IRQn);

    bus_state = OFF_BUS;

    // Reset bxCAN peripheral (set RESET bit to 1)
    hcan.Instance->MCR |= CAN_MCR_RESET;
    // Wait until sleep mode is reached
    while ((hcan.Instance->MSR & CAN_MSR_SLAK) == 0);

    led_off(LED_ERROR);
}


void can_set_bitrate(enum can_bitrate bitrate) {
    if (bus_state == ON_BUS) {
        // cannot set bitrate while on bus
        return;
    }

    switch (bitrate) {
    case CAN_BITRATE_10K:
        prescaler = CAN_PRESCALER_10K;
        break;
    case CAN_BITRATE_20K:
        prescaler = CAN_PRESCALER_20K;
        break;
    case CAN_BITRATE_50K:
        prescaler = CAN_PRESCALER_50K;
        break;
    case CAN_BITRATE_100K:
        prescaler = CAN_PRESCALER_100K;
        break;
    case CAN_BITRATE_125K:
        prescaler = CAN_PRESCALER_125K;
        break;
    case CAN_BITRATE_250K:
        prescaler = CAN_PRESCALER_250K;
        break;
    case CAN_BITRATE_500K:
        prescaler = CAN_PRESCALER_500K;
        break;
    case CAN_BITRATE_750K:
        prescaler = CAN_PRESCALER_750K;
        break;
    default:
//    case CAN_BITRATE_1000K:
        prescaler = CAN_PRESCALER_1000K;
        break;
    }
}


void can_set_filter(uint32_t id, uint32_t mask) {
    CAN_FilterConfTypeDef filter;

    // see page 825 of RM0091 for details on filters
    // set the standard ID part
    filter.FilterIdHigh = (id & 0x7FF) << 5;
    // add the top 5 bits of the extended ID
    filter.FilterIdHigh += (id >> 24) & 0xFFFF;
    // set the low part to the remaining extended ID bits
    filter.FilterIdLow = ((id & 0x1FFFF800) << 3);

    // set the standard ID part
    filter.FilterMaskIdHigh = (mask & 0x7FF) << 5;
    // add the top 5 bits of the extended ID
    filter.FilterMaskIdHigh += (mask >> 24) & 0xFFFF;
    // set the low part to the remaining extended ID bits
    filter.FilterMaskIdLow = ((mask & 0x1FFFF800) << 3);

    filter.FilterMode = CAN_FILTERMODE_IDMASK;
    filter.FilterScale = CAN_FILTERSCALE_32BIT;
    filter.FilterNumber = 0;
    filter.FilterFIFOAssignment = CAN_FIFO0;
    filter.BankNumber = 0;
    filter.FilterActivation = ENABLE;

    HAL_CAN_ConfigFilter(&hcan, &filter);
}


void can_set_silent(uint8_t silent) {
    if (bus_state == ON_BUS) {
        // cannot set silent mode while on bus
        return;
    }
    if (silent) {
        hcan.Init.Mode = CAN_MODE_SILENT;
    } else {
        hcan.Init.Mode = CAN_MODE_NORMAL;
    }
}


void can_process() {
    if (bus_state != ON_BUS)
        return;

    process_rx_frame_buffer();
    process_rx_serial_buffer();
    process_tx_serial_buffer();
    process_tx_frame_buffer();
}
