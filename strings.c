/**
 * @brief Functions for string manipulation.
 */

#include "strings.h"


/**
 * @brief Convert number to string.
 * @param number Numer to convert.
 * @param result Result of conversation.
 * @retval Code of completed operation, zero if all is ok.
 */
int number2string(unsigned short int number, char * result)
{
    unsigned char reversed_numbers[6] = {0, 0, 0, 0, 0, 0};
    int i = 0;
    
    while(number > 10)
    {
        reversed_numbers[i] = number % 10;
        number = number / 10;
        i++;
    }
    reversed_numbers[i] = number;
    i++;

    for(int j = 0; j < i; j++)
    {
        result[j] = reversed_numbers[i - j - 1] + '0';
    }

    return 0;
}

/**
 * @brief Get null-terminated string length (without null-symbol).
 * @param string Null-terminated string.
 * @retval String length.
 */
int size(char * string)
{
    int size = 0;
    while(*string != 0)
    {
        size++;
        string++;
    }
    return size;
}
