/************************************************************************************************
 *
 *  @author     Agustín Jesús Vazquez <vazqueza193@gmail.com>
 *  @date       Marzo, 2025
 *  @version    1.0
 *
 *  @license    MIT License
 *
Copyright (c) 2025, Agustín Jesús Vazquez <vazqueza193@gmail.com>


Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*************************************************************************************************/

/** @file  BME680.cpp
 ** @brief Implementación de la clase BME680
 **/

/* === Headers files inclusions ================================================================ */
#include "BME680.h"
#include <string.h>
/* === Macros definitions ====================================================================== */
// I2C Configuration
#define I2C_MASTER_NUM        I2C_NUM_0           // Puerto I2C
#define I2C_MASTER_SDA_IO     GPIO_NUM_8          // Pin SDA (ajusta según tu hardware)
#define I2C_MASTER_SCL_IO     GPIO_NUM_9          // Pin SCL (ajusta según tu hardware)
#define I2C_MASTER_FREQ_HZ    400000              // Frecuencia de reloj I2C (400 kHz)

#define BME680_REG_CTRL_HUM   0x72
#define BME680_REG_CTRL_MEAS  0x74
#define BME680_REG_HUM_MSB    0x25
#define BME680_REG_HUM_LSB    0x26
#define BME680_MODE_FORCED    0x01

#define BME680_OSR_SKIPPED    0x00
#define BME680_OSR_1X         0x01
#define BME680_OSR_2X         0x02
#define BME680_OSR_4X         0x03
#define BME680_OSR_8X         0x04
#define BME680_OSR_16X        0x05

#define BME680_MODE_FORCED    0x01
/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

BME680::BME680(uint8_t sensor_addr)
{
    i2c = new I2C(I2C_MASTER_NUM, I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO, I2C_MASTER_FREQ_HZ);
    i2c->i2cSetup();
    // Parámetros de calibración para humedad
    par_h1 = par_h2 = par_h3 = par_h4 = par_h5 = par_h7 = par_h6 = 0;

    // Parámetros de calibración para temperatura
    par_t1 = par_t2 = par_t3 = 0;

    // Parámetros de calibración para presión
    par_p1 = par_p2 = par_p3 = par_p4, par_p5 = par_p6, par_p7 = par_p8 = par_p9 = par_p10 = 0;
}

BME680::~BME680() {
    // La instancia de I2C se gestiona externamente.
}

esp_err_t BME680::calibration() {
    // Leer las calibraciones de temperatura, presión y humedad.
    readTemperatureCalibrationData();
    readPressureCalibrationData();
    readHumidityCalibrationData();
    return ESP_OK;
}

esp_err_t BME680::configForcedMode(uint8_t humOSR, uint8_t tempOSR, uint8_t presOSR) {
    esp_err_t err;
    uint8_t data;
    
    // Configurar oversampling de humedad
    err = i2c->readRegister(address, BME680_REG_CTRL_HUM, &data);
    if (err != ESP_OK) {
        ESP_LOGE("BME680", "Error leyendo ctrl_hum");
        return err;
    }
    data &= 0xF8; // Limpiar bits [2:0]
    data |= (humOSR & 0x07);
    err = i2c->writeRegister(address, BME680_REG_CTRL_HUM, &data, 1);
    if (err != ESP_OK) {
        ESP_LOGE("BME680", "Error escribiendo ctrl_hum");
        return err;
    }
    
    // Configurar oversampling de temperatura, presión y modo forced en ctrl_meas
    data = (((tempOSR & 0x07) << 5) | ((presOSR & 0x07) << 2) | BME680_MODE_FORCED);
    err = i2c->writeRegister(address, BME680_REG_CTRL_MEAS, &data, 1);
    if (err != ESP_OK) {
        ESP_LOGE("BME680", "Error escribiendo ctrl_meas");
        return err;
    }
    return ESP_OK;
}

void BME680::readTemperatureCalibrationData() {

    uint8_t calib_data[5];
    memset(calib_data, 0, sizeof(calib_data));
    
    i2c->readRegister(address, 0xE9, &calib_data[0]); // T1 LSB
    i2c->readRegister(address, 0xEA, &calib_data[1]); // T1 MSB
    i2c->readRegister(address, 0x8A, &calib_data[2]); // T2 LSB
    i2c->readRegister(address, 0x8B, &calib_data[3]); // T2 MSB
    i2c->readRegister(address, 0x8C, &calib_data[4]); // T3
    
    par_t1 = (calib_data[1] << 8) | calib_data[0];
    par_t2 = (int16_t)((calib_data[3] << 8) | calib_data[2]);
    par_t3 = (int8_t)calib_data[4];

    ESP_LOGI("BME680", "Calibración Temp: T1=%u, T2=%d, T3=%d",
             par_t1, par_t2, par_t3);
}

uint32_t BME680::readRawTemperature() {
    uint8_t temp_msb = 0, temp_lsb = 0, temp_xlsb = 0;
    
    if (i2c->readRegister(address, 0x22, &temp_msb) != ESP_OK) return 0;
    if (i2c->readRegister(address, 0x23, &temp_lsb) != ESP_OK) return 0;
    if (i2c->readRegister(address, 0x24, &temp_xlsb) != ESP_OK) return 0;
    
    return  (((uint32_t)temp_msb << 12) | ((uint32_t)temp_lsb << 4) | (temp_xlsb >> 4));
}

float BME680::compensateTemperature(uint32_t raw_temp, int32_t *t_fine) {
    float var1 = 0.0, var2 = 0.0;
    
    var1 = ((float)raw_temp / 16384.0f - (float)par_t1 / 1024.0f) * (float)par_t2;
    var2 = (((float)raw_temp / 131072.0f - (float)par_t1 / 8192.0f) *
            ((float)raw_temp / 131072.0f - (float)par_t1 / 8192.0f)) * ((float)par_t3 * 16.0f);
    *t_fine = (int32_t)(var1 + var2);
    
    return (((float)*t_fine) / 5120.0f);
}

void BME680::readPressureCalibrationData() {
    uint8_t p1_lsb = 0, p1_msb = 0;
    i2c->readRegister(address, 0x8E, &p1_lsb);
    i2c->readRegister(address, 0x8F, &p1_msb);
    par_p1 = (p1_msb << 8) | p1_lsb;
    
    uint8_t p2_lsb = 0, p2_msb = 0;
    i2c->readRegister(address, 0x90, &p2_lsb);
    i2c->readRegister(address, 0x91, &p2_msb);
    par_p2 = (int16_t)((p2_msb << 8) | p2_lsb);
    
    i2c->readRegister(address, 0x92, (uint8_t *)&par_p3); // par_p3 es int8_t
    
    uint8_t p4_lsb = 0, p4_msb = 0;
    i2c->readRegister(address, 0x94, &p4_lsb);
    i2c->readRegister(address, 0x95, &p4_msb);
    par_p4 = (int16_t)((p4_msb << 8) | p4_lsb);
    
    uint8_t p5_lsb = 0, p5_msb = 0;
    i2c->readRegister(address, 0x96, &p5_lsb);
    i2c->readRegister(address, 0x97, &p5_msb);
    par_p5 = (int16_t)((p5_msb << 8) | p5_lsb);
    
    i2c->readRegister(address, 0x99, (uint8_t *)&par_p6); // par_p6 es int8_t
    i2c->readRegister(address, 0x98, (uint8_t *)&par_p7); // par_p7 es int8_t
    
    uint8_t p8_lsb = 0, p8_msb = 0;
    i2c->readRegister(address, 0x9C, &p8_lsb);
    i2c->readRegister(address, 0x9D, &p8_msb);
    par_p8 = (int16_t)((p8_msb << 8) | p8_lsb);
    
    uint8_t p9_lsb = 0, p9_msb = 0;
    i2c->readRegister(address, 0x9E, &p9_lsb);
    i2c->readRegister(address, 0x9F, &p9_msb);
    par_p9 = (int16_t)((p9_msb << 8) | p9_lsb);
    
    i2c->readRegister(address, 0xA0, &par_p10); // par_p10 es uint8_t
}

uint32_t BME680::readRawPressure() {
    uint8_t msb, lsb, xlsb;
    
    if(i2c->readRegister(address, 0x1F, &msb) != ESP_OK) return 0;
    if(i2c->readRegister(address, 0x20, &lsb) != ESP_OK) return 0;
    if(i2c->readRegister(address, 0x21, &xlsb) != ESP_OK) return 0;

    return ((uint32_t)msb << 12) | ((uint32_t)lsb << 4) | (xlsb >> 4);
    
}

float BME680::compensatePressure(uint32_t raw_press, int32_t t_fine) {
    double var1, var2, var3, press_comp;

    var1 = ((double)t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double)par_p6 / 131072.0);
    var2 = var2 + (var1 * (double)par_p5 * 2.0);
    var2 = (var2 / 4.0) + ((double)par_p4 * 65536.0);
    var1 = ((((double)par_p3 * var1 * var1) / 16384.0) + ((double)par_p2 * var1)) / 524288.0;
    var1 = (1.0 + (var1 / 32768.0)) * (double)par_p1;
    press_comp = 1048576.0 - (double)raw_press;
    press_comp = ((press_comp - (var2 / 4096.0)) * 6250.0) / var1;
    var1 = ((double)par_p9 * press_comp * press_comp) / 2147483648.0;
    var2 = press_comp * ((double)par_p8 / 32768.0);
    var3 = (press_comp / 256.0) * (press_comp / 256.0) * (press_comp / 256.0) * ((double)par_p10 / 131072.0);
    press_comp = press_comp + (var1 + var2 + var3 + ((double)par_p7 * 128.0)) / 16.0;
    
    return ((float)press_comp / 100.0);
}

// ============================
// Funciones de Humedad
// ============================
uint16_t BME680::readRawHumidity() {
    uint8_t msb, lsb;

    if(i2c->readRegister(address, BME680_REG_HUM_MSB, &msb) != ESP_OK) return 0;
    if(i2c->readRegister(address, BME680_REG_HUM_LSB, &lsb) != ESP_OK) return 0;
    
    return ((uint16_t)msb << 8) | lsb;
}

void BME680::readHumidityCalibrationData() {
    uint8_t buf_E2 = 0, buf_E7 = 0, h1_low = 0, h1_high = 0, h2_high = 0, h2_low = 0;
    
    // Leer H1: 0xE2 y 0xE3
    i2c->readRegister(address, 0xE2, &buf_E2);
    h1_low = buf_E2 & 0x0F;  // Bits [3:0]
    i2c->readRegister(address, 0xE3, &h1_high);
    par_h1 = (h1_high << 4) | h1_low;
    
    // Leer H2: 0xE1 y bits [7:4] de 0xE2
    i2c->readRegister(address, 0xE1, &h2_high);
    h2_low = (buf_E2 >> 4) & 0x0F;
    par_h2 = (int16_t)((h2_high << 4) | h2_low);
    
    // Leer H3 a H7
    i2c->readRegister(address, 0xE4, (uint8_t *)&par_h3);
    i2c->readRegister(address, 0xE5, (uint8_t *)&par_h4);
    i2c->readRegister(address, 0xE6, (uint8_t *)&par_h5);
    i2c->readRegister(address, 0xE7, &buf_E7);
    par_h6 = buf_E7;
    i2c->readRegister(address, 0xE8, (uint8_t *)&par_h7);
}

float BME680::compensateHumidity(uint16_t hum_adc, float temp_comp) {
    double var1, var2, var3, var4;

    var1 = hum_adc - (((double)par_h1 * 16.0) + (((double)par_h3 / 2.0) * temp_comp));
    var2 = var1 * (((double)par_h2 / 262144.0) *
            (1.0 + (((double)par_h4 / 16384.0) * temp_comp) +
             (((double)par_h5 / 1048576.0) * temp_comp * temp_comp)));
    var3 = (double)par_h6 / 16384.0;
    var4 = (double)par_h7 / 2097152.0;
    
    return (var2 + ((var3 + (var4 * temp_comp)) * var2 * var2));
}


bool BME680::getMeasure(measure_t *data) {
    
    esp_err_t err;
    uint16_t hum_raw;
    uint32_t temp_raw, press_raw;
    
    int32_t t_fine;

    readTemperatureCalibrationData();
    readHumidityCalibrationData();
    readPressureCalibrationData();
    
    err = configForcedMode(BME680_OSR_1X, BME680_OSR_2X, BME680_OSR_16X);
    if (err != ESP_OK) {
        return false;
    }

    // Leer y compensar temperatura
    temp_raw = readRawTemperature();
    data->Temperature = compensateTemperature(temp_raw, &t_fine);

    // Leer y compensar humedad
    hum_raw = readRawHumidity();
    data->Humidity = compensateHumidity(hum_raw, data->Temperature);

    // Leer y compensar presión
    press_raw = readRawPressure();
    data->Pressure = compensatePressure(press_raw, t_fine);

    return true;
    
}