#define I2C I2CMock
#include "MockI2C.h"
#undef I2C

#include "BME680.h"
#include <unity.h>

void test_temperature_compensation() {
    BME680 sensor;

    uint32_t raw_temp = 519888;
    int32_t t_fine = 0;
    float temp = sensor.compensateTemperature(raw_temp, &t_fine);

    TEST_ASSERT_FLOAT_WITHIN(10.0, 25.0, temp);
}

void test_pressure_compensation() {
    BME680 sensor;

    uint32_t raw_press = 500000;
    int32_t t_fine = 128000;
    float press = sensor.compensatePressure(raw_press, t_fine);

    TEST_ASSERT_TRUE(press > 300 && press < 1100);
}

void test_humidity_compensation() {
    BME680 sensor;

    float temp_comp = 25.0;
    uint16_t raw_hum = 30000;
    float hum = sensor.compensateHumidity(raw_hum, temp_comp);

    TEST_ASSERT_TRUE(hum >= 0 && hum <= 100);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_temperature_compensation);
    RUN_TEST(test_pressure_compensation);
    RUN_TEST(test_humidity_compensation);
    return UNITY_END();
}
