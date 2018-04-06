// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/**
 * @brief Display driver.
 */

#include "display.h"
#include "numbers.h"
#include "stm32f10x.h"
#include "system_clock.h"


#define SPI_SEND_SUCCESSFULLY 0
#define SPI_SEND_BUSY 1

#define ADDR_DIGIT0 0x0100
#define ADDR_DIGIT1 0x0200
#define ADDR_DIGIT2 0x0300
#define ADDR_DIGIT3 0x0400
#define ADDR_DECODE_MODE 0x0900
#define ADDR_INTENSITY 0x0a00
#define ADDR_SCAN_LIMIT 0x0b00
#define ADDR_SHUTDOWN 0x0c00

volatile static unsigned short int spi_tx_fifo[TX_FIFO_SIZE];
volatile static unsigned int spi_tx_fifo_write_pointer;
volatile static unsigned int spi_tx_fifo_read_pointer;

static int display_write_register(unsigned short int address,
                                  unsigned short int data);
static void init_spi(void);
static void enable_spi(void);


// High level functions:

#define CODE_B_DIGITS3_0 0x0f
#define HALF_INTENSITY 0x08
#define DISPLAY_DIGITS_0123 0x03
#define NORMAL_OPERATION 0x01

static unsigned char brightness_level = 0;

/**
 * @brief Initializes display (MAX7219 driver).
 */
void init_display(void)
{
    spi_tx_fifo_write_pointer = 0;
    spi_tx_fifo_read_pointer = 0;
    init_spi();

    // Wait, while SPI and MAX7219 power up and initializes:
    delay_x10ms(1);
    
    display_write_register(ADDR_DECODE_MODE, CODE_B_DIGITS3_0);
    display_write_register(ADDR_INTENSITY, HALF_INTENSITY);
    display_write_register(ADDR_SCAN_LIMIT, DISPLAY_DIGITS_0123);

    // Need less than 250 microseconds to leave shutdown mode.

    // Set default brightness level after restart to maximum:
    // TODO: Need to save brightness level to E2PROM and restore it here!
    brightness_level = 0x0f;
    display_write_register(ADDR_INTENSITY, brightness_level);

    display_write_register(ADDR_SHUTDOWN, NORMAL_OPERATION);
    delay_x10ms(1);
}

/**
 * @brief Send number to display.
 * @param number Number to show on the display.
 * @return Result of sending number to MAX7219.
 */
int display_send_number(unsigned short int number)
{
    int digits[4] = {0, 0, 0, 0};
    unsigned short int address[4] = {ADDR_DIGIT0,
                                     ADDR_DIGIT1,
                                     ADDR_DIGIT2,
                                     ADDR_DIGIT3};

    if(number2digits_array(number, digits) != 0)
    {
        return DISPLAY_WRITE_CONVERT_FAIL;
    }

    for(int i = 0; i < 4; i++)
    {
        int result = display_write_register(address[i], digits[i]);
        if(result != 0)
        {
            return DISPLAY_WRITE_FAIL;
        }
    }
    return DISPLAY_WRITE_SUCCESS;
}

/**
 * @brief Increase display brightness.
 */
void display_increase_brightness(void)
{
    if(brightness_level < 0x0f)
    {
        brightness_level++;
    }
        
    display_write_register(ADDR_INTENSITY, brightness_level);
}

/**
 * @brief Decrease display brightness.
 */
void display_decrease_brightness(void)
{
    if(brightness_level > 0x00)
    {
        brightness_level--;
    }

    display_write_register(ADDR_INTENSITY, brightness_level);
}

#define SPI_TX_FIFO_OVERFLOW 1

/**
 * @brief Send one command to the MAX7219.
 * @param address Address in MAX7219.
 * @param data Data to send to MAX7219.
 * @retval Result of sending command, zero if all ok.
 */
static int display_write_register(unsigned short int address,
                                  unsigned short int data)
{
    __disable_irq();
    if(spi_tx_fifo_write_pointer >= TX_FIFO_SIZE)
    {
        __enable_irq();
        return SPI_TX_FIFO_OVERFLOW;
    }

    spi_tx_fifo[spi_tx_fifo_write_pointer] = (address & 0xff00) | (data & 0x00ff);
    spi_tx_fifo_write_pointer++;
    enable_spi();
    __enable_irq();
    return 0;
}


// Low-level functions:

/**
 * @brief Initializes SPI.
 */
static void init_spi(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    /* Initialize next SPI1 pins:
     * PA4 - SPI1_NSS
     * PA5 - SPI1_SCK
     * PA7 - SPI1_MOSI
     */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
    // Push-pull outputs:
    GPIOA->CRL &= ~GPIO_CRL_CNF4;
    GPIOA->CRL |= GPIO_CRL_CNF4_1;
    GPIOA->CRL |= GPIO_CRL_MODE4_0;
    
    GPIOA->CRL &= ~GPIO_CRL_CNF5;
    GPIOA->CRL |= GPIO_CRL_CNF5_1;
    GPIOA->CRL |= GPIO_CRL_MODE5_0;

    GPIOA->CRL &= ~GPIO_CRL_CNF7;
    GPIOA->CRL |= GPIO_CRL_CNF7_1;
    GPIOA->CRL |= GPIO_CRL_MODE7_0;

    // Setup clock baudrate in BR[2:0] (f_PCLK/256).
    SPI1->CR1 |= 0x07 << 3;
    // Use 16-bit data frame format.
    SPI1->CR1 |= ((unsigned short int)0x0001) << 11;
    // MSB first.
    // SPI half-duplex, transmit only (BIDIMODE=0, RXONLY=0)
    // NSS pin using, set SSOE bit.
    SPI1->CR2 |= 0x01 << 2;
    // Set MSTR bit.
    SPI1->CR1 |= 0x01 << 2;
    // Unmask TXE interrupt.
    SPI1->CR2 |= 0x01 << 7;
}

/**
 * @brief Unmask TXE interrupt and enable SPI.
 */
static inline void enable_spi(void)
{
    SPI1->CR1 |= 0x01 << 6;
    NVIC_EnableIRQ(SPI1_IRQn);
}

/**
 * @brief Mask TXE interrupt and disable SPI.
 */
static inline void disable_spi(void)
{
    NVIC_DisableIRQ(SPI1_IRQn);
    SPI1->CR1 &= ~((unsigned short int)0x0001 << 6);
}

void SPI1_IRQHandler(void)
{
    if((SPI1->SR & SPI_SR_TXE) &&
       ((SPI1->SR & SPI_SR_BSY) == 0x00))
    {
        if(spi_tx_fifo_read_pointer > 0)
        {
            // Set LOAD to high.
            disable_spi();
        }
            
        // Sending next data chunk via SPI:
        if(spi_tx_fifo_read_pointer < spi_tx_fifo_write_pointer)
        {
            enable_spi();
            SPI1->DR = spi_tx_fifo[spi_tx_fifo_read_pointer];
            spi_tx_fifo_read_pointer++;
            return;
        }
        else if(spi_tx_fifo_read_pointer >= spi_tx_fifo_write_pointer)
        {
            spi_tx_fifo_read_pointer = 0;
            spi_tx_fifo_write_pointer = 0;
            disable_spi();
        }
    }
}
