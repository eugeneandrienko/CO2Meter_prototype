/**
 * @brief Functions for numbers manipulation.
 */

#ifndef NUMBERS_H
#define NUMBERS_H

/**
 * @brief Convert number to array of digits.
 * @param number Number to convert.
 * @param result Result of conversation - array of 4 integers, LSB in zero index, MSB last.
 * @retval Code of completed operation, zero if all is ok.
 */
int number2digits_array(unsigned short int number, int * result);

#endif // NUMBERS_H
