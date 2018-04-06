/**
 * @brief Display driver.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#define TX_FIFO_SIZE 64

#define DISPLAY_WRITE_SUCCESS 0
#define DISPLAY_WRITE_FAIL 1
#define DISPLAY_WRITE_CONVERT_FAIL 2

/**
 * @brief Initializes display (MAX7219 driver).
 */
void init_display(void);

/**
 * @brief Send number to display.
 * @param number Number to show on the display.
 * @return Result of sending number to MAX7219.
 */
int display_send_number(unsigned short int number);

/**
 * @brief Increase display brightness.
 */
void display_increase_brightness(void);

/**
 * @brief Decrease display brightness.
 */
void display_decrease_brightness(void);

#endif // DISPLAY_H
