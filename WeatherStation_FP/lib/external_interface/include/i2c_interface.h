#ifndef I2C_INTERFACE_H
#define I2C_INTERFACE_H

#include "esp_err.h"


class I2C_interface {       
    
    public:

        virtual esp_err_t i2cSetup() = 0;
    
        virtual esp_err_t readRegister(uint8_t addr, uint8_t reg, uint8_t *data) = 0;

        virtual esp_err_t writeRegister(uint8_t addr, uint8_t reg, uint8_t *data, size_t len) = 0;
    };   

#endif // I2C_INTERFACE_H