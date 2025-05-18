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
#include "BME680_defines.h"  
#include <string.h>
#include <cstdio>
// #include "esp_log.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

/**
 * @brief Constructor de la clase BME680.
 *
 * @param sensor_addr Dirección I2C del sensor BME680.
 */
BME680::BME680(I2C_interface *i2c_driver, uint8_t sensor_addr) 
{
    //i2c = new I2C(I2C_MASTER_NUM, I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO, I2C_MASTER_FREQ_HZ);
    i2c = i2c_driver; // Se utiliza la instancia de I2C proporcionada externamente.
    address = sensor_addr;
    i2c->i2cSetup();

    // Parámetros de calibración para temperatura
    par_t1 = par_t2 = par_t3 = 0;

    // Parámetros de calibración para humedad
    par_h1 = par_h2 = par_h3 = par_h4 = 
    par_h5 = par_h6 = par_h7 = 0;

    // Parámetros de calibración para presión
    par_p1 = par_p2 = par_p3 = par_p4 =
    par_p5 = par_p6 = par_p7 = par_p8 =
    par_p9 = par_p10 = 0;

}

/**
 * @brief Destructor de la clase BME680.
 *
 * Libera recursos asignados (excepto I2C, gestionado externamente).
 */
BME680::~BME680() {
    // La instancia de I2C se gestiona externamente.
}

/**
 * @brief Realiza la calibración del sensor BME680.
 *
 * @return ESP_OK si la calibración fue exitosa.
 */
esp_err_t BME680::calibration() {
    // Leer las calibraciones de temperatura, presión y humedad.
    readTemperatureCalibrationData();
    readPressureCalibrationData();
    readHumidityCalibrationData();
    return ESP_OK;
}

/**
 * @brief Configura el sensor en modo forced y establece oversampling para humedad, temperatura y presión.
 *
 * @param humOSR  Oversampling para humedad.
 * @param tempOSR Oversampling para temperatura.
 * @param presOSR Oversampling para presión.
 * 
 * @return ESP_OK si la configuración fue exitosa, error en caso contrario.
 */
esp_err_t BME680::configForcedMode(uint8_t humOSR, uint8_t tempOSR, uint8_t presOSR) {
    esp_err_t err;
    uint8_t data;
    
    err = i2c->readRegister(this->address, BME680_REG_CTRL_HUM, &data);
    if (err != ESP_OK) {
        // ESP_LOGE("BME680", "Error leyendo ctrl_hum");
        return err;
    }
    data &= 0xF8; // Limpiar bits [2:0]
    data |= (humOSR & 0x07);
    err = i2c->writeRegister(this->address, BME680_REG_CTRL_HUM, &data, 1);
    if (err != ESP_OK) {
        // ESP_LOGE("BME680", "Error escribiendo ctrl_hum");
        return err;
    }
    
    // Configurar oversampling de temperatura, presión y modo forced en ctrl_meas
    data = (((tempOSR & 0x07) << 5) | ((presOSR & 0x07) << 2) | BME680_MODE_FORCED);
    err = i2c->writeRegister(this->address, BME680_REG_CTRL_MEAS, &data, 1);
    if (err != ESP_OK) {
        // ESP_LOGE("BME680", "Error escribiendo ctrl_meas");
        return err;
    }
    // vTaskDelay(pdMS_TO_TICKS(50)); // Esperar 50 ms para que el sensor realice la medición
    return ESP_OK;
}

/**
 * @brief Lee los datos de calibración de temperatura desde el sensor.
 */
void BME680::readTemperatureCalibrationData() {
    uint8_t calib_data[5];
    memset(calib_data, 0, sizeof(calib_data));
    
    i2c->readRegister(this->address, BME680_REG_T1_MSB, &calib_data[0]);
    i2c->readRegister(this->address, BME680_REG_T1_LSB, &calib_data[1]);
    i2c->readRegister(this->address, BME680_REG_T2_MSB, &calib_data[2]);
    i2c->readRegister(this->address, BME680_REG_T2_LSB, &calib_data[3]);
    i2c->readRegister(this->address, BME680_REG_T3, &calib_data[4]);
    
    par_t1 = (calib_data[1] << 8) | calib_data[0];
    par_t2 = (int16_t)((calib_data[3] << 8) | calib_data[2]);
    par_t3 = (int8_t)calib_data[4];
}

/**
 * @brief Realiza una lectura del valor crudo (raw) de la temperatura.
 *
 * @return Valor crudo de temperatura obtenido del sensor.
 */
uint32_t BME680::readRawTemperature() {
    uint8_t temp_msb = 0, temp_lsb = 0, temp_xlsb = 0;
    
    if (i2c->readRegister(this->address, BME680_REG_TRAW_MSB, &temp_msb) != ESP_OK) return 0;
    if (i2c->readRegister(this->address, BME680_REG_TRAW_LSB, &temp_lsb) != ESP_OK) return 0;
    if (i2c->readRegister(this->address, BME680_REG_TRAW_XLSB, &temp_xlsb) != ESP_OK) return 0;
    
    return  (((uint32_t)temp_msb << 12) | ((uint32_t)temp_lsb << 4) | (temp_xlsb >> 4));
}

/**
 * @brief Calcula y compensa la temperatura a partir del valor crudo leído.
 *
 * @param raw_temp Valor crudo de temperatura leído del sensor.
 * @param t_fine Valor intermedio necesario para otras compensaciones (presión).
 * 
 * @return Temperatura compensada en grados Celsius.
 */
float BME680::compensateTemperature(uint32_t raw_temp, int32_t *t_fine) {
    float var1, var2;
    
    var1 = ((float)raw_temp / 16384.0f - (float)par_t1 / 1024.0f) * (float)par_t2;
    var2 = (((float)raw_temp / 131072.0f - (float)par_t1 / 8192.0f) *
            ((float)raw_temp / 131072.0f - (float)par_t1 / 8192.0f)) * ((float)par_t3 * 16.0f);
    *t_fine = (int32_t)(var1 + var2);
    
    return (((float)*t_fine) / 5120.0f);
}

/**
 * @brief Lee los datos de calibración de presión desde el sensor.
 */
void BME680::readPressureCalibrationData() {
    uint8_t p1_lsb = 0, p1_msb = 0;
    i2c->readRegister(this->address, BME680_REG_P1_LSB, &p1_lsb);
    i2c->readRegister(this->address, BME680_REG_P1_MSB, &p1_msb);
    par_p1 = (p1_msb << 8) | p1_lsb;
    
    uint8_t p2_lsb = 0, p2_msb = 0;
    i2c->readRegister(this->address, BME680_REG_P2_LSB, &p2_lsb);
    i2c->readRegister(this->address, BME680_REG_P2_MSB, &p2_msb);
    par_p2 = (int16_t)((p2_msb << 8) | p2_lsb);
    
    i2c->readRegister(this->address, BME680_REG_P3, (uint8_t *)&par_p3); // par_p3 es int8_t
    
    uint8_t p4_lsb = 0, p4_msb = 0;
    i2c->readRegister(this->address, BME680_REG_P4_LSB, &p4_lsb);
    i2c->readRegister(this->address, BME680_REG_P4_MSB, &p4_msb);
    par_p4 = (int16_t)((p4_msb << 8) | p4_lsb);
    
    uint8_t p5_lsb = 0, p5_msb = 0;
    i2c->readRegister(this->address, BME680_REG_P5_LSB, &p5_lsb);
    i2c->readRegister(this->address, BME680_REG_P5_MSB, &p5_msb);
    par_p5 = (int16_t)((p5_msb << 8) | p5_lsb);
    
    i2c->readRegister(this->address, BME680_REG_P6, (uint8_t *)&par_p6); // par_p6 es int8_t
    i2c->readRegister(this->address, BME680_REG_P7, (uint8_t *)&par_p7); // par_p7 es int8_t
    
    uint8_t p8_lsb = 0, p8_msb = 0;
    i2c->readRegister(this->address, BME680_REG_P8_LSB, &p8_lsb);
    i2c->readRegister(this->address, BME680_REG_P8_MSB, &p8_msb);
    par_p8 = (int16_t)((p8_msb << 8) | p8_lsb);
    
    uint8_t p9_lsb = 0, p9_msb = 0;
    i2c->readRegister(this->address, BME680_REG_P9_LSB, &p9_lsb);
    i2c->readRegister(this->address, BME680_REG_P9_MSB, &p9_msb);
    par_p9 = (int16_t)((p9_msb << 8) | p9_lsb);
    
    i2c->readRegister(this->address, BME680_REG_P10, &par_p10); // par_p10 es uint8_t
}

/**
 * @brief Realiza una lectura del valor crudo (raw) de presión.
 *
 * @return Valor crudo de presión obtenido del sensor.
 */
uint32_t BME680::readRawPressure() {
    uint8_t msb, lsb, xlsb;
    
    if(i2c->readRegister(this->address, BME680_REG_PRAW_MSB, &msb) != ESP_OK) return 0;
    if(i2c->readRegister(this->address, BME680_REG_PRAW_LSB, &lsb) != ESP_OK) return 0;
    if(i2c->readRegister(this->address, BME680_REG_PRAW_XLSB, &xlsb) != ESP_OK) return 0;

    return ((uint32_t)msb << 12) | ((uint32_t)lsb << 4) | (xlsb >> 4);
    
}

/**
 * @brief Calcula y compensa la presión a partir del valor crudo leído.
 *
 * @param raw_press Valor crudo de presión leído del sensor.
 * @param t_fine Valor intermedio proveniente de la compensación de temperatura.
 * 
 * @return Presión compensada en hectopascales (hPa).
 */
float BME680::compensatePressure(uint32_t raw_press, int32_t t_fine) {
    double var1, var2, var3, press_comp;

    // Protección contra división por cero
    if (par_p1 == 0) {
        return -1; // Error: par_p1 no puede ser cero
    }

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

/**
 * @brief Lee los datos de calibración de humedad desde el sensor.
 */
void BME680::readHumidityCalibrationData() {
    // uint8_t buf_E2, h1_low, h1_high, h2_high, h2_low;

    uint8_t h1_lsb, h1_msb;
    uint8_t h2_msb, h2_lsb;

    // --- H1: 12 bits en E2 (LSB bits[3:0]) + E3 (MSB bits[7:0]) ---
    i2c->readRegister(this->address, BME680_REG_H1_LSB, &h1_lsb);   // 0xE2
    i2c->readRegister(this->address, BME680_REG_H1_MSB, &h1_msb);   // 0xE3
    par_h1 = (uint16_t)((h1_msb << 4) | (h1_lsb & 0x0F));

    // --- H2: 12 bits en E1 (MSB bits[7:0]) + E2 (LSB bits[7:4]) ---
    i2c->readRegister(this->address, BME680_REG_H2_MSB, &h2_msb);   // 0xE1
    i2c->readRegister(this->address, BME680_REG_H2_LSB, &h2_lsb);   // 0xE2 (reuse buf)
    par_h2 = (int16_t)((h2_msb << 4) | (h2_lsb >> 4));
    
    // // Leer H1: 0xE2 y 0xE3
    // i2c->readRegister(this->address, BME680_REG_H1_LSB, &buf_E2);
    // h1_low = buf_E2 & 0x0F;  // Bits [3:0]
    // i2c->readRegister(this->address, BME680_REG_H1_MSB, &h1_high);
    // par_h1 = (h1_high << 4) | h1_low;
    
    // // Leer H2: 0xE1 y bits [7:4] de 0xE2
    // i2c->readRegister(this->address, BME680_REG_H1_MSB, &h2_high);
    // h2_low = (buf_E2 >> 4) & 0x0F;
    // par_h2 = (int16_t)((h2_high << 4) | h2_low);
    
    i2c->readRegister(this->address, BME680_REG_H3, (uint8_t *)&par_h3);
    i2c->readRegister(this->address, BME680_REG_H4, (uint8_t *)&par_h4);
    i2c->readRegister(this->address, BME680_REG_H5, (uint8_t *)&par_h5);
    i2c->readRegister(this->address, BME680_REG_H6, (uint8_t *)&par_h6);
    i2c->readRegister(this->address, BME680_REG_H7, (uint8_t *)&par_h7);
}

/**
 * @brief Realiza una lectura del valor crudo (raw) de humedad.
 *
 * @return Valor crudo de humedad obtenido del sensor.
 */
uint16_t BME680::readRawHumidity() {
    uint8_t msb, lsb;

    if(i2c->readRegister(this->address, BME680_REG_HRAW_MSB, &msb) != ESP_OK) return 0;
    if(i2c->readRegister(this->address, BME680_REG_HRAW_LSB, &lsb) != ESP_OK) return 0;
    
    return ((uint16_t)msb << 8) | lsb;
}

/**
 * @brief Calcula y compensa la humedad a partir del valor crudo leído.
 *
 * @param hum_adc Valor crudo de humedad leído del sensor.
 * @param temp_comp Temperatura compensada previamente calculada.
 * 
 * @return Humedad compensada en porcentaje (%).
 */
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

/**
 * @brief Realiza un ciclo completo de medición y compensación de temperatura, humedad y presión.
 *
 * @param data Estructura para almacenar los resultados compensados de las mediciones.
 * @return true si la medición y compensación fueron exitosas, false en caso contrario.
 */
bool BME680::getMeasure(measure_t *data) {
    uint16_t hum_raw;
    uint32_t temp_raw, press_raw;
    
    int32_t t_fine;

    calibration();
    
    if(configForcedMode(BME680_OSR_2X, BME680_OSR_4X, BME680_OSR_16X) != ESP_OK) {
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

/* === End of documentation ==================================================================== */