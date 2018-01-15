/**
 * @file
 * @brief This file implements functions to configure and use the Controller Area Network (CAN) bus
 */

#include "stm32f0xx_hal.h"
#include "can.h"
#include "led.h"

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
static enum can_bus_state bus_state;


void can_init(void) {
    // Default speed: 1 Mbps
    prescaler = CAN_PRESCALER_1000K;
    hcan.Instance = CAN;
    bus_state = OFF_BUS;
}

void can_set_filter(uint32_t id, uint32_t mask) {
    CAN_FilterConfTypeDef filter;

    // see page 825 of RM0091 for details on filters
    // set the standard ID part
    filter.FilterIdHigh = (id & 0x7FF) << 5;
    // add the top 5 bits of the extended ID
    filter.FilterIdHigh += (id >> 24) & 0xFFFF;
    // set the low part to the remaining extended ID bits
    filter.FilterIdLow += ((id & 0x1FFFF800) << 3);

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

    if (bus_state == ON_BUS) {
        HAL_CAN_ConfigFilter(&hcan, &filter);
    }
}

void can_enable(void) {
    if (bus_state == OFF_BUS) {
        hcan.Init.Prescaler = prescaler;
        hcan.Init.Mode = CAN_MODE_NORMAL;
        hcan.Init.SJW = CAN_SJW;
        hcan.Init.BS1 = CAN_BS1;
        hcan.Init.BS2 = CAN_BS2;
        hcan.Init.TTCM = DISABLE;
        hcan.Init.ABOM = DISABLE;
        hcan.Init.AWUM = DISABLE;
        hcan.Init.NART = DISABLE;
        hcan.Init.RFLM = DISABLE;
        hcan.Init.TXFP = DISABLE;
        hcan.pTxMsg = NULL;
        HAL_CAN_Init(&hcan);
        bus_state = ON_BUS;
        can_set_filter(0, 0);
    }
}

void can_disable(void) {
    if (bus_state == ON_BUS) {
        // do a bxCAN reset (set RESET bit to 1)
        hcan.Instance->MCR |= CAN_MCR_RESET;
        bus_state = OFF_BUS;
    }
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
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

uint32_t can_tx(CanTxMsgTypeDef *tx_msg, uint32_t timeout) {
    uint32_t status;

    // transmit can frame
    hcan.pTxMsg = tx_msg;
    status = HAL_CAN_Transmit(&hcan, timeout);

    led_on();
    return status;
}

uint32_t can_rx(CanRxMsgTypeDef *rx_msg, uint32_t timeout) {
    uint32_t status;

    hcan.pRxMsg = rx_msg;

    status = HAL_CAN_Receive(&hcan, CAN_FIFO0, timeout);

    led_on();
    return status;
}

uint8_t is_can_msg_pending(uint8_t fifo) {
    if (bus_state == OFF_BUS) {
        return 0;
    }
    return (__HAL_CAN_MSG_PENDING(&hcan, fifo) > 0);
}
