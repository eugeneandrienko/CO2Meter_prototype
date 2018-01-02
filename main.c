/**
  @file    main.c
  @author  h0rr0rr_drag0n
  @version V0.0.1
  @date    22-Aug-2011
  @brief   Template of Linux project for STM32VLDiscovery
**/

#include "stm32f10x.h"
#include "drivers/system_clock.h"
#include "drivers/uart.h"


/**
   @brief  Enable clocking on various system peripheral devices
   @params None
   @retval None
**/
void RCC_init()
{
    /* enable clocking on Port C */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

/**
 * @brief Initializes system.
 */
void init()
{
    __enable_irq();
    init_system_clock();
    init_uart();
}

int main(void)
{
    GPIO_InitTypeDef GPIOC_init_params;

    RCC_init();

    /* Blue LED sits on PC[8] and Green LED sits on PC[9]*/
    GPIOC_init_params.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    /* Output maximum frequency selection is 10 MHz.
       Do not worry that internal oscillator of STM32F100RB
       works on 8MHz frequency - Cortex-M3 core has a various
       facilities to carefully tune the frequency for almost
       peripheral devices.
    */
    GPIOC_init_params.GPIO_Speed = GPIO_Speed_10MHz;
    /* Push-pull output.
       Двухтактный выход.
    */
    GPIOC_init_params.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIOC_init_params);

    /* Set pins 8 and 9 at PORTC to low level */
    GPIO_ResetBits(GPIOC, GPIO_Pin_8);
    GPIO_ResetBits(GPIOC, GPIO_Pin_9);

    init();

    while(1)
    {
        if(send_number(1234) == UART_ERROR_TX_FIFO_OVERFLOW)
        {
            GPIO_SetBits(GPIOC, GPIO_Pin_9);
        }
    }
}
