#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <cstdint>
#include "esp_log.h"
#include "esp_err.h"
#include "driver/i2c.h"

void i2c_init(void);
esp_err_t i2c_read(uint8_t address, uint8_t *data_rd, size_t size);
esp_err_t i2c_write(uint8_t address, uint8_t *data_wr, size_t size);
uint8_t bme680_read_chip_id(void);

#ifdef __cplusplus
}
#endif

#endif // MAIN_H
