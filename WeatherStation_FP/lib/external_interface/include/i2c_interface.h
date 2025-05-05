#ifndef I2C_INTERFACE_H
#define I2C_INTERFACE_H

#include "esp_err.h"


class I2C_interface {
    private:
        // i2c_port_t i2c_num;       
        // gpio_num_t sda_io;        
        // gpio_num_t scl_io;        
        // uint32_t clk_speed;         
    
    public:

        // I2C(i2c_port_t _i2c_num, gpio_num_t _sda_io, gpio_num_t _scl_io, uint32_t _clk_speed);
    
        // ~I2C();

        virtual esp_err_t i2cSetup() = 0;
    
        virtual esp_err_t readRegister(uint8_t addr, uint8_t reg, uint8_t *data) = 0;

        virtual esp_err_t writeRegister(uint8_t addr, uint8_t reg, uint8_t *data, size_t len) = 0;
    };   

#endif // I2C_INTERFACE_H