#include <unity.h>
#include "demo.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_add(void) {
    TEST_ASSERT_EQUAL(2, add(1, 1));
    TEST_ASSERT_EQUAL(4, add(2, 2));
    TEST_ASSERT_EQUAL(0, add(-1, 1));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_add);
    return UNITY_END();
}
