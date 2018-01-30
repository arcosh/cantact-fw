
#ifndef USART_H
#define USART_H

/**
 * Initialize U(S)ART interface to PC
 */
void uart_init();


/*
 * Low-level i/o functions for newlib, e.g. for printf
 */

/**
 * Read chars from USART input
 */
int _read(int file, char *ptr, int len);

/**
 * Write chars to PC via USART
 *
 * @param file: File/pipe to write to; here disregarded
 * @param ptr:  Pointer to buffer to write to UART
 * @param len:  Length of write buffer in bytes
 */
int _write(int file, char *ptr, int len);

#endif
