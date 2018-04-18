
#include <buffering.h>

#include <stdint.h>
#include <stdbool.h>
#include "platform.h"
#include "config.h"
#include "fifo.h"
#include "can.h"
#include "slcan.h"
#include "led.h"
#include "clock.h"


extern CAN_HandleTypeDef hcan;

/**
 * Secondary buffer for received CAN frames
 */
can_frame_t can_rx_frame_buffer[CAN_RX_FRAME_BUFFER_SIZE];
fifo_t can_rx_frame_fifo;

/**
 * Buffer for serialized received CAN frames
 */
uint8_t can_rx_serial_buffer[CAN_RX_SERIAL_BUFFER_SIZE];
fifo_t can_rx_serial_fifo;

/**
 * Buffer for outgoing CAN frames in serial (SLCAN) format
 */
uint8_t can_tx_serial_buffer[CAN_TX_SERIAL_BUFFER_SIZE];
fifo_t can_tx_serial_fifo;

/**
 * Buffer for outgoing CAN frames
 */
can_frame_t can_tx_frame_buffer[CAN_TX_FRAME_BUFFER_SIZE];
fifo_t can_tx_frame_fifo;


void buffering_init()
{
    fifo_init(&can_rx_frame_fifo,   (uint8_t*) can_rx_frame_buffer,     CAN_RX_FRAME_BUFFER_SIZE);
    fifo_init(&can_rx_serial_fifo,  can_rx_serial_buffer,               CAN_RX_SERIAL_BUFFER_SIZE);
    fifo_init(&can_tx_serial_fifo,  can_tx_serial_buffer,               CAN_TX_SERIAL_BUFFER_SIZE);
    fifo_init(&can_tx_frame_fifo,   (uint8_t*) can_tx_frame_buffer,     CAN_TX_FRAME_BUFFER_SIZE);
}


void process_rx_frame_buffer()
{
    if (fifo_is_empty(&can_rx_frame_fifo))
        // Nothing in source FIFO
        return;

    if (can_rx_frame_fifo.element_count == 0)
        // No frames in source FIFO
        return;

    if (!fifo_has_room(&can_rx_serial_fifo, SLCAN_MTU))
        // Not enough room in target FIFO
        return;

    // Pop one frame from FIFO
    can_frame_t frame;
    fifo_pop(&can_rx_frame_fifo, &frame, sizeof(can_frame_t));

    // Convert frame to SLCAN string
    uint8_t slcan_frame[SLCAN_MTU];
    uint8_t length;
    slcan_parse_frame(&frame, &slcan_frame, &length);

    // Append SLCAN string to buffer
    fifo_push(&can_rx_serial_fifo, slcan_frame, length);
}


void process_rx_serial_buffer()
{
    if (fifo_is_empty(&can_rx_frame_fifo))
        // Nothing in source FIFO
        return;

    if (can_rx_frame_fifo.element_count == 0)
        // No frames in source FIFO
        return;

    if (!USB ready for transmission)
        // Not enough room in target FIFO
        return;

    // Retrieve oldest SLCAN-encoded frame from reception buffer
    uint8_t buffer[SLCAN_MTU];
    fifo_pop(&can_rx_serial_fifo, buffer, length);

    // Transmit SLCAN string to PC

    // via USB
    #ifdef PC_INTERFACE_USB
    uint8_t result = CDC_Transmit_FS(buffer, length);
    if (result == USBD_OK)
    {
        led_on(LED_ACTIVITY);
    }
    else
    {
        led_on(LED_ERROR);
    }
    #endif

    // via UART
    #ifdef PC_INTERFACE_UART
    _write(0, (char*) buffer, length);
    led_on(LED_ACTIVITY);
    #endif
}


void process_tx_serial_buffer()
{
    if (can_tx_serial_fifo.element_count == 0)
        return;

    if (!fifo_has_room(&can_tx_frame_fifo, sizeof(can_frame_t)))
        return;

    uint8_t slcan_frame[SLCAN_MTU];
    can_frame_t frame;
    convert_slcan_to_frame(&slcan_frame, &frame);

    fifo_push(&can_tx_frame_fifo, (uint8_t*) (&frame), sizeof(can_frame_t));
}


void process_tx_frame_buffer()
{
    if ((hcan.Instance->TSR & CAN_TSR_TME) == 0)
        // No free transmitter mailbox available
        return;

    if (fifo_is_empty(&can_tx_frame_fifo))
        // No frames ready for transmission
        return;

    // Pop one frame from the FIFO
    can_frame_t frame;
    fifo_pop(&can_rx_frame_fifo, (uint8_t*) (&frame), sizeof(can_frame_t));

    if (frame.dlc > 8)
        // Invalid frame; discard
        return;

    // Convert simple frame to HAL frame
    static CanTxMsgTypeDef can_tx_frame;
    // TODO: Implement support for extended IDs and RTR frames
    can_tx_frame.RTR = CAN_RTR_DATA;
    can_tx_frame.IDE = CAN_ID_STD;
    can_tx_frame.StdId = frame.id;
    // Copy data
    can_tx_frame.DLC = frame.dlc;
    for (uint8_t i=0; i<frame.dlc; i++)
    {
        can_tx_frame.Data[i] = frame.data[i];
    }

    // Select empty transmit mailbox
    uint8_t transmitmailbox;
    if ((hcan.Instance->TSR & CAN_TSR_TME0) == CAN_TSR_TME0)
    {
        transmitmailbox = 0;
    }
    else if ((hcan.Instance->TSR & CAN_TSR_TME1) == CAN_TSR_TME1)
    {
        transmitmailbox = 1;
    }
    else if ((hcan.Instance->TSR & CAN_TSR_TME2) == CAN_TSR_TME2)
    {
        transmitmailbox = 2;
    }
    else
    {
        // No free transmit mailbox
        // We should never arrive here due to the above check.
        return;
    }

    /* Set up the  */
    hcan.Instance->sTxMailBox[transmitmailbox].TIR = 0;
    if (can_tx_frame.IDE == CAN_ID_STD)
    {
        hcan.Instance->sTxMailBox[transmitmailbox].TIR |= ((can_tx_frame.StdId << 21) | can_tx_frame.RTR);
    }
    else
    {
        hcan.Instance->sTxMailBox[transmitmailbox].TIR |= ((can_tx_frame.ExtId << 3) | can_tx_frame.IDE | can_tx_frame.RTR);
    }

    /* Set up the DLC */
    hcan.Instance->sTxMailBox[transmitmailbox].TDTR = can_tx_frame.DLC;

    /* Set up the data field */
    hcan.Instance->sTxMailBox[transmitmailbox].TDLR =
                                            (((uint32_t)can_tx_frame.Data[3] << 24) |
                                             ((uint32_t)can_tx_frame.Data[2] << 16) |
                                             ((uint32_t)can_tx_frame.Data[1] << 8) |
                                             ((uint32_t)can_tx_frame.Data[0]));
    hcan.Instance->sTxMailBox[transmitmailbox].TDHR =
                                            (((uint32_t)can_tx_frame.Data[7] << 24) |
                                             ((uint32_t)can_tx_frame.Data[6] << 16) |
                                             ((uint32_t)can_tx_frame.Data[5] << 8) |
                                             ((uint32_t)can_tx_frame.Data[4]));

    // Request transmission
    hcan.Instance->sTxMailBox[transmitmailbox].TIR |= CAN_TI0R_TXRQ;

    /*
     * CAN transmission error and timeout checking
     */
    static bool timeout_enabled[3] = {false, false, false};
    static uint32_t timeout_start_ms[3];

    timeout_start_ms[transmitmailbox] = get_time();
    timeout_enabled[transmitmailbox] = true;

    const uint32_t error_flag[3] = {CAN_TSR_TERR0, CAN_TSR_TERR1, CAN_TSR_TERR2};
    const uint32_t arbitration_lost_flag[3] = {CAN_TSR_ALST0, CAN_TSR_ALST1, CAN_TSR_ALST2};
    const uint32_t abort_transmission_switch[3] = {CAN_TSR_ABRQ0, CAN_TSR_ABRQ1, CAN_TSR_ABRQ2};

    for (uint8_t i=0; i<3; i++)
    {
        bool abort = false;

        // If an error other than arbitration-lost occurs, abort transmission.
        if ((hcan.Instance->TSR & error_flag[i])
         &&(!(hcan.Instance->TSR & arbitration_lost_flag[i])))
        {
            abort = true;
        }
        // If a pending transmission takes too long, cancel it.
        else if (timeout_enabled[i] && (get_time() - timeout_start_ms[i] > CAN_TX_TIMEOUT))
        {
            abort = true;
        }

        if (abort)
        {
            // Abort transmission
            hcan.Instance->TSR |= abort_transmission_switch[i];
            timeout_enabled[i] = false;
            led_on(LED_ERROR);
        }
    }
}

