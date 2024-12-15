#ifndef I2C_H
#define I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_log.h"
#include "esp_err.h"

void i2c_init(void);
esp_err_t i2c_read(uint8_t address, uint8_t *data_rd, size_t size);
esp_err_t i2c_write(uint8_t address, uint8_t *data_wr, size_t size);

#ifdef __cplusplus
}
#endif

#endif // I2C_H