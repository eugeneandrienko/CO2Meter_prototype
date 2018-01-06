/**
 * @brief System clock.
 */

#include <limits.h>
#include "stm32f10x.h"
#include "system_clock.h"
#include "uart.h"
#include "mhz19b.h"


// Generate timer interrupt every 10 ms.
#define SYSTICK_PRELOADED (8000000UL/100 - 1)

volatile static unsigned char systicks;


/**
 * @brief Initializes system clock.
 * @retval None
 */
void init_system_clock(void)
{
    systicks = 0;
    SysTick_Config(SYSTICK_PRELOADED);
}

/**
 * @brief Delays system execution for x10 ms.
 * @param x10ms Milliseconds to sleep (x10).
 */
void delay_x10ms(unsigned char x10ms)
{
    unsigned char systicks_first = systicks;
    unsigned char systicks_current = systicks_first;

    while(1)
    {
        if(((systicks_current < systicks_first) &&
            ((UCHAR_MAX - systicks_first) + systicks_current >= x10ms)) ||
           (systicks_current - systicks_first >= x10ms))
        {
            return;
        }

        systicks_current = systicks;
    }
}


#define SYSTICK_USART 1
#define SYSTICK_CO2_SENSOR 2
#define SYSTICK_DISPLAY 3

static unsigned int systick_fsm_state = SYSTICK_USART;

void SysTick_Handler(void)
{
    systicks++;
    
    /* FSM description:
     *
     * USART -> CO2_SENSOR -> DISPLAY
     *   ^                       |
     *   |_______________________|
     */
    switch(systick_fsm_state)
    {
    case SYSTICK_USART:
        send_byte_usart1();
        systick_fsm_state = SYSTICK_CO2_SENSOR;
        break;
    case SYSTICK_CO2_SENSOR:
        systick_fsm_state = SYSTICK_DISPLAY;
        break;
    case SYSTICK_DISPLAY:
        systick_fsm_state = SYSTICK_USART;
        break;
    default:
        systick_fsm_state = SYSTICK_USART;
    }
}
