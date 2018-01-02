/**
 * @brief Definition of UART driver for communication with other world.
 */

#ifndef UART_H
#define UART_H

#define TX_FIFO_SIZE 64

#define UART_ERROR_TX_FIFO_OVERFLOW 1
#define UART_ERROR_INTERNAL 2


/**
 * @brief Initializes UART to communicate with other world.
 */
void init_uart(void);

/**
 * @brief Sends number value via UART.
 * @param ppm_value PPM value to send via UART.
 * @return Zero, if byte successfully added to queue. Otherwise, returns non-zero.
 */
int send_number(unsigned short int ppm_value);


/**
 * @brief Low-level send byte method for USART1.
 * @params None
 * @retval None
 */
void send_byte_usart1(void);

#endif //UART_H
