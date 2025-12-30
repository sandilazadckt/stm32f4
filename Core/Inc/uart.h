#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "stm32f401xe.h"

/**
 * @brief Initialize UART1 for debug communication
 *
 * Configures:
 * - GPIOA pins PA9 (TX) and PA10 (RX)
 * - UART1 peripheral at 115200 baud rate
 * - 8 data bits, 1 stop bit, no parity
 */
void uart_debug_init(void);

/**
 * @brief Transmit a single byte over UART
 *
 * @param ch The byte to transmit
 */
void uart_write(uint8_t ch);

/**
 * @brief Receive a single byte from UART (blocking)
 *
 * @return The received byte
 */
uint8_t uart_read(void);

/**
 * @brief Transmit a null-terminated string over UART
 *
 * @param str Pointer to the null-terminated string
 */
void uart_write_string(const char *str);

/**
 * @brief Non-blocking UART read with timeout
 *
 * @param data Pointer to store the received byte
 * @param timeout Maximum number of polling iterations
 * @return 1 if data was received, 0 if timeout occurred
 */
uint8_t uart_read_timeout(uint8_t *data, uint32_t timeout);

/**
 * @brief Check if UART data is available to read
 *
 * @return 1 if data is available, 0 otherwise
 */
uint8_t uart_data_available(void);

#endif // UART_H
