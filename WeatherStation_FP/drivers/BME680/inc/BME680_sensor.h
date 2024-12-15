#ifndef BME680_SENSOR_H
#define BME680_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <cstdint>

uint8_t bme680_read_chip_id(void);

#ifdef __cplusplus
}
#endif

#endif // BME680_SENSOR_H

