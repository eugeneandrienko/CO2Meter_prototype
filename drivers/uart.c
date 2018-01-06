/**
 * @brief UART driver for communication with other world.
*/

#include "stm32f10x.h"
#include "uart.h"
#include "strings.h"


volatile static unsigned char uart_tx_fifo[TX_FIFO_SIZE];
volatile static unsigned int uart_tx_fifo_write_pointer;
volatile static unsigned int uart_tx_fifo_read_pointer;

static void init_usart1(void);
static int send_byte(unsigned char byte);
static int tx_fifo_free_space(void);


// High level functions:

/**
 * @brief Initializes UART
 */
void init_uart(void)
{
    __disable_irq();
    uart_tx_fifo_write_pointer = 0;
    uart_tx_fifo_read_pointer = 0;
    __enable_irq();
    init_usart1();
}

/**
 * @brief Sends number value via UART.
 * @param ppm_value PPM value to send via UART.
 * @return Zero, if byte successfully added to queue. Otherwise, returns non-zero.
 */
int send_number(unsigned short int ppm_value)
{
    char ppm_string[6] = {0, 0, 0, 0, 0, 0};
    if(number2string(ppm_value, ppm_string) != 0)
    {
        return UART_ERROR_INTERNAL;
    }
    
    int ppm_length = size(ppm_string);
    if(tx_fifo_free_space() < (ppm_length + 1))
    {
        return UART_ERROR_TX_FIFO_OVERFLOW;
    }

    int retval = 0;
    char * ppm_string_pointer = ppm_string;
    while(*ppm_string_pointer != '\0')
    {
        retval = send_byte(*ppm_string_pointer);
        if(retval != 0)
        {
            return retval;
        }
        ppm_string_pointer++;
    }

    retval = send_byte('\0');
    return retval;
}

/**
 * @brief Sends byte via UART.
 * @param byte Byte to send.
 * @return Zero, if byte successfully added to queue. Otherwise, returns non-zero.
 */
static int send_byte(unsigned char byte)
{
    __disable_irq();
    if(uart_tx_fifo_write_pointer >= TX_FIFO_SIZE)
    {
        // Buffer overflow but not all bytes sent via UART:
        __enable_irq();
        return UART_ERROR_TX_FIFO_OVERFLOW;
    }
    
    uart_tx_fifo[uart_tx_fifo_write_pointer] = byte;
    uart_tx_fifo_write_pointer++;
    __enable_irq();
    return 0;
}

/**
 * @brief Returns free bytes in FIFO.
 * @param None
 * @return Free bytes in FIFO.
 */
static int tx_fifo_free_space(void)
{
    __disable_irq();
    int result = TX_FIFO_SIZE - uart_tx_fifo_write_pointer;
    __enable_irq();
    return result;
}


// Low-level functions:

/**
 * @brief Initializes USART1.
 * @retval None
 */
static void init_usart1(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    /* Initialize next USART pins:
     * PA9  - USART_TX
     * PA10 - USART_RX
     */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
    GPIOA->CRH &= ~GPIO_CRH_CNF9;
    GPIOA->CRH |= GPIO_CRH_CNF9_1;
    GPIOA->CRH |= GPIO_CRH_MODE9_0;
    GPIOA->CRH &= ~GPIO_CRH_CNF10;
    GPIOA->CRH |= GPIO_CRH_CNF10_0;
    GPIOA->CRH &= ~GPIO_CRH_MODE10;

    /* 9600 baud rate on the 8 MHz APB2 bus */
    // (8 000 000 / 9600)/16 = 52.08333(3)
    // 52 = 0x34
    // 0.08333(3) * 16 = 1.33(3) = 0x1
    USART1->BRR = 0x341;
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

/**
 * @brief Low-level send byte method for USART1.
 * @retval None
 */
void send_byte_usart1(void)
{
    if(uart_tx_fifo_write_pointer > 0 &&
       uart_tx_fifo_read_pointer < uart_tx_fifo_write_pointer)
    {
        USART1->DR = uart_tx_fifo[uart_tx_fifo_read_pointer];
        uart_tx_fifo_read_pointer++;
    }
    else if(uart_tx_fifo_read_pointer >= uart_tx_fifo_write_pointer)
    {
        uart_tx_fifo_read_pointer = 0;
        uart_tx_fifo_write_pointer = 0;
    }
}
