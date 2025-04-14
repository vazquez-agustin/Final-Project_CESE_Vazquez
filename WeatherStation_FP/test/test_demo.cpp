#include <unity.h>
#include "demo.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_demo_returns_one(void) {
    Demo demo;
    TEST_ASSERT_EQUAL(1, demo.F());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_demo_returns_one);
    return UNITY_END();
}
