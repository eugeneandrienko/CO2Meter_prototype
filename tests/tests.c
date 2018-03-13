#include <stdio.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "strings_test.h"
#include "numbers_test.h"


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
    CU_pSuite strSuite = NULL;
    CU_pSuite numSuite = NULL;

    if(CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }

    strSuite = CU_add_suite("strings_test_suite", init_suite, clean_suite);
    numSuite = CU_add_suite("numbers_test_suite", init_suite, clean_suite);
    if(NULL == strSuite || NULL == numSuite)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if((NULL == CU_add_test(strSuite, "test_number2string", test_number2string)) ||
       (NULL == CU_add_test(strSuite, "test_size", test_size)) ||
       (NULL == CU_add_test(strSuite, "test_empty_string_size", test_empty_string_size)) ||
       (NULL == CU_add_test(numSuite, "test_number2digits_array", test_number2digits_array)))
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
