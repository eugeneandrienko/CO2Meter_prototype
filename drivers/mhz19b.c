/**
 * @brief Driver for MH-Z19B CO2 sensor.
 */

#include "mhz19b.h"
#include "stm32f10x.h"
#include "system_clock.h"


#define COMMAND_LENGTH 7 // Command length (without start byte and checksum)
#define REQUEST_LENGTH 9
#define RESPONSE_LENGTH 9

#define RANGE_2000 2000
#define RANGE_5000 5000

static void abc_logic_state(int state);
static void detection_range(int range);
static int send_command(unsigned char command[REQUEST_LENGTH]);
static unsigned char get_checksum(unsigned char command[COMMAND_LENGTH]);
static void init_usart2(void);
static void enable_txe_interrupt(void);

volatile static unsigned char mhz19b_tx_fifo[REQUEST_LENGTH];
volatile static unsigned int mhz19b_tx_fifo_pointer;
volatile static unsigned char mhz19b_rx_fifo[RESPONSE_LENGTH];
volatile static unsigned int mhz19b_rx_fifo_pointer;


// High level functions:

/**
 * @brief Initializes MH-Z19B sensor.
 */
void init_mhz19b(void)
{
    __disable_irq();
    mhz19b_tx_fifo_pointer = REQUEST_LENGTH;
    mhz19b_rx_fifo_pointer = 0;
    __enable_irq();
    
    init_usart2();

    // Wait 5 seconds, while module initializing
    delay_x10ms(200);
    delay_x10ms(200);
    delay_x10ms(100);

    detection_range(RANGE_5000);
    delay_x10ms(50);
    abc_logic_state(0);
    delay_x10ms(50);
}

#define ABC_LOGIC_BYTE 3
#define ABC_ENABLED 0xA0
#define ABC_DISABLED 0x00
#define CHECKSUM_BYTE 8

/**
 * @brief Set state of internal ABC logic.
 * @param state Zero if ABC off, non-zero if ABC is on.
 */
static void abc_logic_state(int state)
{
    unsigned char command[REQUEST_LENGTH] = {
        0xFF, 0x01, 0x79,
        0x00, // ABC state
        0x00, 0x00, 0x00, 0x00,
        0x00 // checksum
    };
    if(state)
    {
        command[ABC_LOGIC_BYTE] = ABC_ENABLED;
        command[CHECKSUM_BYTE] = 0xE6;
    }
    else
    {
        command[ABC_LOGIC_BYTE] = ABC_DISABLED;
        command[CHECKSUM_BYTE] = 0x86;
    }

    // Should send this command at any cost.
    // If command not sent - MH-Z19B not initialized
    // and is in inconsistent state and cannot be used.
    while(send_command(command) != 0) {}
}

#define RANGE_HI_BYTE 3
#define RANGE_LO_BYTE 4

/**
 * @brief Set's sensor detection range.
 * @param range Can be 2000 or 5000 PPM.
 */
static void detection_range(int range)
{
    unsigned char command[REQUEST_LENGTH] = {
        0xFF, 0x01, 0x99,
        0x00, 0x00, // detection range
        0x00, 0x00, 0x00,
        0x00 // checksum
    };

    if(range == RANGE_2000)
    {
        command[RANGE_HI_BYTE] = 0x07;
        command[RANGE_LO_BYTE] = 0xD0;
        command[CHECKSUM_BYTE] = 0x8F;
    }
    else if(range == RANGE_5000)
    {
        command[RANGE_HI_BYTE] = 0x13;
        command[RANGE_LO_BYTE] = 0x88;
        command[CHECKSUM_BYTE] = 0xCB;
    }

    // Should send this command at any cost.
    // If command not sent - MH-Z19B not initialized
    // and is in inconsistent state and cannot be used.
    while(send_command(command) != 0) {}
}

/**
 * @brief Send requset of CO2 concentration.
 * @retval Non-zero if request failed.
 */
int request_co2_concentration(void)
{
    unsigned char command[REQUEST_LENGTH] = {
        0xFF, 0x01, 0x86,
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x79
    };

    return send_command(command);
}

#define SENSOR_NO_BYTE 1
#define SENSOR_NO 0x86

#define CONCENTRATION_HI 1
#define CONCENTRATION_LO 2

int read_co2_concentration(void)
{
    __disable_irq();
    if(mhz19b_rx_fifo_pointer < RESPONSE_LENGTH)
    {
        __enable_irq();
        return RESPONSE_NOT_RECEIVED;
    }

    mhz19b_rx_fifo_pointer = 0;
    
    if(mhz19b_rx_fifo[SENSOR_NO_BYTE] != SENSOR_NO)
    {
        __enable_irq();
        return RESPONSE_WRONG_SENSOR_NO;
    }

    unsigned char command[COMMAND_LENGTH] = {
        mhz19b_rx_fifo[1], mhz19b_rx_fifo[2], mhz19b_rx_fifo[3],
        mhz19b_rx_fifo[4], mhz19b_rx_fifo[5], mhz19b_rx_fifo[6],
        mhz19b_rx_fifo[7]
    };
    unsigned char actual_checksum = mhz19b_rx_fifo[CHECKSUM_BYTE];
    __enable_irq();

    unsigned char expected_checksum = get_checksum(command);

    if(actual_checksum != expected_checksum)
    {
        return RESPONSE_WRONG_CHECKSUM;
    }

    unsigned short int concentration = 0x00ff & command[CONCENTRATION_HI];
    concentration <<= 8;
    concentration |= (0x00ff & command[CONCENTRATION_LO]);
    return concentration;
}

/**
 * @brief Send 9-byte command via UART to MH-Z19B.
 * @param command 9-byte command.
 * @retval Result of sending command. Zero if all is successful.
 */
static int send_command(unsigned char command[REQUEST_LENGTH])
{
    int result = 0;
    __disable_irq();
    
    if(mhz19b_tx_fifo_pointer >= REQUEST_LENGTH)
    {
        // Already send all data, could send new one:
        mhz19b_tx_fifo_pointer = 0;
        enable_txe_interrupt();
        for(int i = 0; i < REQUEST_LENGTH; i++)
        {
            mhz19b_tx_fifo[i] = command[i];
        }
    }
    else
    {
        // Some data is still sending:
        result = MHZ19B_PREVIOUS_COMMAND_NOT_SENT;
    }

    __enable_irq();
    return result;
}

/**
 * @brief Get checksum for given 7 bytes for sensor.
 * @param command Array with 7 bytes of command (without start byte).
 * @retval Calculated checksum.
 */
static unsigned char get_checksum(unsigned char command[COMMAND_LENGTH])
{
    unsigned char checksum = 0;
    for(int i = 0; i < COMMAND_LENGTH; i++)
    {
        checksum += command[i];
    }
    checksum = ~checksum;
    checksum++;
    return checksum;
}


// Low-level functions:

static void init_usart2(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    /* Initialize next USART pins:
     * PA2 - USART_TX
     * PA3 - USART_RX
     */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
    GPIOA->CRL &= ~GPIO_CRL_CNF2;
    GPIOA->CRL |= GPIO_CRL_CNF2_1;
    GPIOA->CRL |= GPIO_CRL_MODE2_0;
    GPIOA->CRL &= ~GPIO_CRL_CNF3;
    GPIOA->CRL |= GPIO_CRL_CNF3_0;
    GPIOA->CRL &= ~GPIO_CRL_MODE3;

    /* 9600 baud rate on the 8 MHz APB1 bus */
    // See same method in uart.c
    USART2->BRR = 0x341;
    USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE |
        USART_CR1_RXNEIE;
    
    NVIC_EnableIRQ(USART2_IRQn);
}

static void enable_txe_interrupt(void)
{
    USART2->CR1 |= USART_CR1_TXEIE;
}

void USART2_IRQHandler(void)
{
    if(USART2->SR & USART_SR_TXE)
    {
        // Can push new byte to the DR register:
        if(mhz19b_tx_fifo_pointer < REQUEST_LENGTH)
        {
            USART2->DR = mhz19b_tx_fifo[mhz19b_tx_fifo_pointer];
            mhz19b_tx_fifo_pointer++;
        }
        else
        {
            // Disable TXE interrupt, while we do not
            // have any data to send.
            USART2->CR1 &= ~USART_CR1_TXEIE;
        }
    }

    if((USART2->SR & USART_SR_RXNE) ||
       (USART2->SR & USART_SR_ORE))
    {
        unsigned char received_byte = USART2->DR;
        if((mhz19b_rx_fifo_pointer == 0) && (received_byte == 0xFF))
        {
            mhz19b_rx_fifo[0] = received_byte;
            mhz19b_rx_fifo_pointer++;
        }
        else if(mhz19b_rx_fifo_pointer < RESPONSE_LENGTH)
        {
            mhz19b_rx_fifo[mhz19b_rx_fifo_pointer] = received_byte;
            mhz19b_rx_fifo_pointer++;
        }
    }
}
