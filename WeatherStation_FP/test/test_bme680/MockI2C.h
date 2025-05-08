#ifndef MOCK_I2C_H
#define MOCK_I2C_H

#include "i2c_interface.h"
#include <stdint.h>
#include "esp_err.h"

class I2CMock: public I2C_interface {
public:
    I2CMock() {}
    ~I2CMock() {}

    esp_err_t i2cSetup() { return ESP_OK; }

    esp_err_t readRegister(uint8_t addr, uint8_t reg, uint8_t* data) {
        *data = 42;
        return ESP_OK;
    }

    esp_err_t writeRegister(uint8_t addr, uint8_t reg, uint8_t* data, size_t len) {
        return ESP_OK;
    }
};

#endif
