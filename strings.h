/**
 * @brief Functions for strings support.
 */

#ifndef STRINGS_H
#define STRINGS_H

/**
 * @brief Convert number to string.
 * @param number Numer to convert.
 * @param result Result of conversation.
 * @retval Code of completed operation, zero if all is ok.
 */
int number2string(unsigned short int number, char * result);

/**
 * @brief Get null-terminated string length (without null-symbol).
 * @param string Null-terminated string.
 * @retval String length.
 */
int size(char * string);

#endif // STRINGS_H
