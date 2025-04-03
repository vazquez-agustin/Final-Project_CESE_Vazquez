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
/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

BME680::BME680(I2C *i2c_driver, uint8_t sensor_addr)
    : i2c(i2c_driver), address(sensor_addr)
{
}

BME680::~BME680() {
    // La instancia de I2C se gestiona externamente.
}

esp_err_t BME680::init() {
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


uint32_t BME680::readRawTemperature() {
    uint8_t temp_msb, temp_lsb, temp_xlsb;
    
    if (i2c->readRegister(address, 0x22, &temp_msb) != ESP_OK) return 0;
    if (i2c->readRegister(address, 0x23, &temp_lsb) != ESP_OK) return 0;
    if (i2c->readRegister(address, 0x24, &temp_xlsb) != ESP_OK) return 0;
    
    uint32_t raw_temp = ((uint32_t)temp_msb << 12) | ((uint32_t)temp_lsb << 4) | (temp_xlsb >> 4);
    ESP_LOGI("BME680", "Raw Temperature: 0x%05lX (%lu)", (unsigned long)raw_temp, (unsigned long)raw_temp);
    return raw_temp;
}

void BME680::readTemperatureCalibrationData() {
    uint8_t calib_data[5];
    
    i2c->readRegister(address, 0xE9, &calib_data[0]); // T1 LSB
    i2c->readRegister(address, 0xEA, &calib_data[1]); // T1 MSB
    i2c->readRegister(address, 0x8A, &calib_data[2]); // T2 LSB
    i2c->readRegister(address, 0x8B, &calib_data[3]); // T2 MSB
    i2c->readRegister(address, 0x8C, &calib_data[4]); // T3
    
    par_t1 = (calib_data[1] << 8) | calib_data[0];
    par_t2 = (int16_t)((calib_data[3] << 8) | calib_data[2]);
    par_t3 = (int8_t)calib_data[4];
    
    ESP_LOGI("BME680", "Calibración Temp: T1=%d, T2=%d, T3=%d", par_t1, par_t2, par_t3);
}

float BME680::compensateTemperature(uint32_t raw_temp, int32_t *t_fine) {
    float var1, var2, temp;
    
    var1 = ((float)raw_temp / 16384.0f - (float)par_t1 / 1024.0f) * (float)par_t2;
    var2 = (((float)raw_temp / 131072.0f - (float)par_t1 / 8192.0f) *
            ((float)raw_temp / 131072.0f - (float)par_t1 / 8192.0f)) * ((float)par_t3 * 16.0f);
    *t_fine = (int32_t)(var1 + var2);
    temp = ((float)*t_fine) / 5120.0f;
    
    return temp;
}

void BME680::readPressureCalibrationData() {
    uint8_t p1_lsb, p1_msb;
    i2c->readRegister(address, 0x8E, &p1_lsb);
    i2c->readRegister(address, 0x8F, &p1_msb);
    par_p1 = (p1_msb << 8) | p1_lsb;
    
    uint8_t p2_lsb, p2_msb;
    i2c->readRegister(address, 0x90, &p2_lsb);
    i2c->readRegister(address, 0x91, &p2_msb);
    par_p2 = (int16_t)((p2_msb << 8) | p2_lsb);
    
    i2c->readRegister(address, 0x92, (uint8_t *)&par_p3); // par_p3 es int8_t
    
    uint8_t p4_lsb, p4_msb;
    i2c->readRegister(address, 0x94, &p4_lsb);
    i2c->readRegister(address, 0x95, &p4_msb);
    par_p4 = (int16_t)((p4_msb << 8) | p4_lsb);
    
    uint8_t p5_lsb, p5_msb;
    i2c->readRegister(address, 0x96, &p5_lsb);
    i2c->readRegister(address, 0x97, &p5_msb);
    par_p5 = (int16_t)((p5_msb << 8) | p5_lsb);
    
    i2c->readRegister(address, 0x99, (uint8_t *)&par_p6); // par_p6 es int8_t
    i2c->readRegister(address, 0x98, (uint8_t *)&par_p7); // par_p7 es int8_t
    
    uint8_t p8_lsb, p8_msb;
    i2c->readRegister(address, 0x9C, &p8_lsb);
    i2c->readRegister(address, 0x9D, &p8_msb);
    par_p8 = (int16_t)((p8_msb << 8) | p8_lsb);
    
    uint8_t p9_lsb, p9_msb;
    i2c->readRegister(address, 0x9E, &p9_lsb);
    i2c->readRegister(address, 0x9F, &p9_msb);
    par_p9 = (int16_t)((p9_msb << 8) | p9_lsb);
    
    i2c->readRegister(address, 0xA0, &par_p10); // par_p10 es uint8_t
    
    ESP_LOGI("BME680", "Calibración Pres: P1=%u, P2=%d, P3=%d, P4=%d, P5=%d, P6=%d, P7=%d, P8=%d, P9=%d, P10=%u",
             par_p1, par_p2, par_p3, par_p4, par_p5, par_p6, par_p7, par_p8, par_p9, par_p10);
}

uint32_t BME680::readRawPressure() {
    uint8_t msb, lsb, xlsb;
    if(i2c->readRegister(address, 0x1F, &msb) != ESP_OK) return 0;
    if(i2c->readRegister(address, 0x20, &lsb) != ESP_OK) return 0;
    if(i2c->readRegister(address, 0x21, &xlsb) != ESP_OK) return 0;
    uint32_t raw_press = ((uint32_t)msb << 12) | ((uint32_t)lsb << 4) | (xlsb >> 4);
    ESP_LOGI("BME680", "Raw Pressure: 0x%05lX (%lu)", (unsigned long)raw_press, (unsigned long)raw_press);
    return raw_press;
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
    // Convertir a hPa (dividir entre 100)
    return (float)press_comp / 100.0;
}

// ============================
// Funciones de Humedad
// ============================
esp_err_t BME680::readRawHumidity(uint16_t *hum_adc) {
    uint8_t msb, lsb;
    esp_err_t err;
    
    err = i2c->readRegister(address, BME680_REG_HUM_MSB, &msb);
    if (err != ESP_OK) {
        ESP_LOGE("BME680", "Error leyendo MSB de humedad");
        return err;
    }
    
    err = i2c->readRegister(address, BME680_REG_HUM_LSB, &lsb);
    if (err != ESP_OK) {
        ESP_LOGE("BME680", "Error leyendo LSB de humedad");
        return err;
    }
    
    *hum_adc = ((uint16_t)msb << 8) | lsb;
    ESP_LOGI("BME680", "Valor de humedad raw: %u", *hum_adc);
    
    return ESP_OK;
}

void BME680::readHumidityCalibrationData() {
    uint8_t buf_E2, buf_E7;
    uint8_t h1_low, h1_high;
    uint8_t h2_high, h2_low;
    
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
    
    ESP_LOGI("BME680", "Calibración Humedad: H1=%u, H2=%d, H3=%d, H4=%d, H5=%d, H6=%u, H7=%d",
             par_h1, par_h2, par_h3, par_h4, par_h5, par_h6, par_h7);
}

float BME680::compensateHumidity(uint16_t hum_adc, float temp_comp) {
    double var1, var2, var3, var4;
    var1 = hum_adc - (((double)par_h1 * 16.0) + (((double)par_h3 / 2.0) * temp_comp));
    var2 = var1 * (((double)par_h2 / 262144.0) *
            (1.0 + (((double)par_h4 / 16384.0) * temp_comp) +
             (((double)par_h5 / 1048576.0) * temp_comp * temp_comp)));
    var3 = (double)par_h6 / 16384.0;
    var4 = (double)par_h7 / 2097152.0;
    float hum_comp = var2 + ((var3 + (var4 * temp_comp)) * var2 * var2);
    return hum_comp;
}