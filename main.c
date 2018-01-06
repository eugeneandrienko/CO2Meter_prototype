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
#include "drivers/mhz19b.h"


/**
   @brief  Enable clocking on various system peripheral devices.
   @retval None
**/
void RCC_init(void)
{
    /* enable clocking on Port C */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

/**
 * @brief Initializes system.
 */
void init(void)
{
    __enable_irq();

    init_system_clock();

    init_uart();
    init_mhz19b();
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
        if(request_co2_concentration() != 0)
        {
            delay_x10ms(5);
            continue;
        }
        
        int result = 0;
        result = read_co2_concentration();
        
        if((result == RESPONSE_WRONG_CHECKSUM) || (result == RESPONSE_WRONG_SENSOR_NO))
        {
            GPIO_SetBits(GPIOC, GPIO_Pin_9);
            continue;
        }
        if(result == RESPONSE_NOT_RECEIVED)
        {
            delay_x10ms(5);
            continue;
        }

        send_number(result);
    }
}
