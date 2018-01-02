/**
 * @brief System clock.
 */

#include "stm32f10x.h"
#include "system_clock.h"
#include "uart.h"


#define SYSTICK_PRELOADED (8000000UL/100 - 1)

/**
 * @brief Initializes system clock.
 * @params None
 * @retval None
 */
void init_system_clock(void)
{
    SysTick_Config(SYSTICK_PRELOADED);
}


#define SYSTICK_USART 1
#define SYSTICK_CO2_SENSOR 2
#define SYSTICK_DISPLAY 3

static unsigned int systick_fsm_state = SYSTICK_USART;

void SysTick_Handler(void)
{
    GPIO_SetBits(GPIOC, GPIO_Pin_8);

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
    
    GPIO_ResetBits(GPIOC, GPIO_Pin_8);
}
