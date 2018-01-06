/**
 * @brief Definitions for System clock.
 */

#ifndef SYSTEM_CLOCK_H
#define SYSTEM_CLOCK_H

/**
 * @brief Initializes system clock.
 * @retval None
 */
void init_system_clock(void);

/**
 * @brief Delays system execution for x10 ms.
 * @param x10ms Milliseconds to sleep (x10).
 */
void delay_x10ms(unsigned char x10ms);

#endif // SYSTEM_CLOCK_H
