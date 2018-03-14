// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/**
 * @brief Functions for number manipulation.
 */

#include "numbers.h"


/**
 * @brief Convert number to array of digits.
 * @param number Number to convert.
 * @param result Result of conversation - array of 4 integers, LSB in zero index, MSB last.
 * @retval Code of completed operation, zero if all is ok.
 */
int number2digits_array(unsigned short int number, int * result)
{
    for(int i = 0; i < 4; i++)
    {
        result[i] = number % 10;
        number = number / 10;
    }
    
    return 0;
}
