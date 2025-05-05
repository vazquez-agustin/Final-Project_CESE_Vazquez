#include "BME680.h"

float BME680::compensateTemperature(uint32_t raw_temp, int32_t *t_fine) {
    float var1, var2;
    par_t1 = 27504;
    par_t2 = 26435;
    par_t3 = -1000;

    var1 = ((float)raw_temp / 16384.0f - (float)par_t1 / 1024.0f) * (float)par_t2;
    var2 = (((float)raw_temp / 131072.0f - (float)par_t1 / 8192.0f) *
            ((float)raw_temp / 131072.0f - (float)par_t1 / 8192.0f)) * ((float)par_t3 * 16.0f);
    *t_fine = (int32_t)(var1 + var2);

    return (((float)*t_fine) / 5120.0f);
}

float BME680::compensatePressure(uint32_t raw_press, int32_t t_fine) {
    return 1000.0;
}

float BME680::compensateHumidity(uint16_t hum_adc, float temp_comp) {
    return 50.0;
}
