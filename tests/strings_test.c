// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "strings.h"
#include "strings_test.h"


void test_number2string(void)
{
    char result[6] = {0, 0, 0, 0, 0, 0};
    int retval = number2string(1234, result);

    CU_ASSERT_EQUAL(retval, 0);
    CU_ASSERT_STRING_EQUAL(result, "1234\0");
}

void test_size(void)
{
    char test_string[] = "testStr\0";
    int result = size(test_string);
    CU_ASSERT_EQUAL(result, 7);
}

void test_empty_string_size(void)
{
    char test_string[] = "\0";
    int result = size(test_string);
    CU_ASSERT_EQUAL(result, 0);
}
