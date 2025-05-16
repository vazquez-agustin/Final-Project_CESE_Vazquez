#ifndef MOCK_I2C_H
#define MOCK_I2C_H

#include "i2c_interface.h"
#include <cstddef>  // for size_t
#include <map>  // for std::map

#define ESP_OK 0

class I2CMock: public I2C_interface {
private:
    std::map<uint8_t,uint8_t> reg_map;

public:
    I2CMock() {}
    ~I2CMock() {}

    esp_err_t i2cSetup() { 
        return ESP_OK; 
    }

    esp_err_t readRegister(uint8_t addr, uint8_t reg, uint8_t* data) {
        *data = reg_map.count(reg) ? reg_map[reg] : 0;
        return ESP_OK;
    }

    esp_err_t writeRegister(uint8_t addr, uint8_t reg, uint8_t* data, size_t len) {
        for(size_t i = 0; i < len; i++) {
            reg_map[reg + i] = data[i];
        }
        return ESP_OK;
    }

    /** Para test: devuelve el valor que hay en un registro concreto */
    uint8_t getRegisterValue(uint8_t reg) const {
        auto it = reg_map.find(reg);
        return it != reg_map.end() ? it->second : 0;
    }
};

#endif
