#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "numbers.h"
#include "numbers_test.h"


void test_number2digits_array(void)
{
    int result[4] = {0, 0, 0, 0};
    int retval = number2digits_array(1234, result);

    CU_ASSERT_EQUAL(retval, 0);
    CU_ASSERT_EQUAL(result[0], 4);
    CU_ASSERT_EQUAL(result[1], 3);
    CU_ASSERT_EQUAL(result[2], 2);
    CU_ASSERT_EQUAL(result[3], 1);
}
