#include "../drivers/BME680/inc/BME680_sensor.h"
#include "../drivers/i2c/inc/i2c.h"
#include "esp_log.h"

#define I2C_ADDRESS 0x76 // Dirección I2C del BME680

static const char *TAG = "BME680";

uint8_t bme680_read_chip_id(void) { //0x76
    uint8_t chip_id = 0;
    i2c_write(I2C_ADDRESS, (uint8_t[]){0xD0}, 1); // Dirección del registro CHIP_ID
    i2c_read(I2C_ADDRESS, &chip_id, 1);

    return chip_id;
}
