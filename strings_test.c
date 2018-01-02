#include <stdio.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "strings.h"


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


int init_suite(void)
{
    return 0;
}

int clean_suite(void)
{
    return 0;
}

int main(void)
{
    CU_pSuite pSuite = NULL;

    if(CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }

    pSuite = CU_add_suite("strings_test_suite", init_suite, clean_suite);
    if(NULL == pSuite)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if((NULL == CU_add_test(pSuite, "test_number2string", test_number2string)) ||
       (NULL == CU_add_test(pSuite, "test_size", test_size)) ||
       (NULL == CU_add_test(pSuite, "test_empty_string_size", test_empty_string_size)))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    printf("\n");
    CU_basic_show_failures(CU_get_failure_list());
    printf("\n\n");

    CU_cleanup_registry();
    return CU_get_error();
}
