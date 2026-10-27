#include <Color.h>
#include <unity.h>

void setUp() {}
void tearDown() {}

void test_black_chromaticity_is_zero()
{
    const Revduino::CIEColor black;
    TEST_ASSERT_EQUAL_FLOAT(0.0f, black.chromaticityX());
    TEST_ASSERT_EQUAL_FLOAT(0.0f, black.chromaticityY());
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_black_chromaticity_is_zero);
    return UNITY_END();
}
