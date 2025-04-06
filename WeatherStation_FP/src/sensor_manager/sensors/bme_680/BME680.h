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

#ifndef BME680_H
#define BME680_H

/** @file  BME680.h
 ** @brief Interfaz pública de la clase BME680.
 **/

/* === Headers files inclusions ================================================================ */
#include "I2C.h"
/* === Public macros definitions =============================================================== */
#define BME680_I2C_ADDRES     0x77 ///< Dirección I2C del sensor BME680.
/* === Public data type declarations =========================================================== */
typedef struct measure {
    float Temperature;
    float Pressure;
    float Humidity;
} measure_t;
/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

class BME680 {
    private:
        I2C *i2c;                ///< Puntero a la instancia de la clase I2C para la comunicación.
        uint8_t address;         ///< Dirección I2C del sensor.
    
        // Parámetros de calibración para humedad
        uint16_t par_h1;
        int16_t par_h2;
        int8_t par_h3, par_h4, par_h5, par_h7;
        uint8_t par_h6;
    
        // Parámetros de calibración para temperatura
        uint16_t par_t1;
        int16_t par_t2;
        int8_t par_t3;
    
        // Parámetros de calibración para presión
        uint16_t par_p1;
        int16_t par_p2;
        int8_t par_p3;
        int16_t par_p4, par_p5;
        int8_t par_p6, par_p7;
        int16_t par_p8, par_p9;
        uint8_t par_p10;
    
    public:
        /**
         * @brief Constructor de la clase BME680.
         *
         * @param i2c_driver Puntero a la instancia de la clase I2C.
         * @param sensor_addr Dirección I2C del sensor.
         */
        BME680(uint8_t sensor_addr = BME680_I2C_ADDRES);
    
        /**
         * @brief Destructor de la clase BME680.
         */
        ~BME680();
    
        /**
         * @brief Inicializa y calibra las variables de los parámetros del sensor BME680.
         *
         * @return esp_err_t Resultado de la operación.
         */
        esp_err_t calibration();
    
        /**
         * @brief Configura el sensor en modo forced con los oversampling especificados.
         *
         * @param humOSR  Oversampling para humedad.
         * @param tempOSR Oversampling para temperatura.
         * @param presOSR Oversampling para presión.
         * 
         * @return esp_err_t Resultado de la operación.
         */
        esp_err_t configForcedMode(uint8_t humOSR, uint8_t tempOSR, uint8_t presOSR);
    
        /**
         * @brief Lee el valor raw de temperatura del sensor.
         *
         * @return uint32_t Valor raw de temperatura.
         */
        uint32_t readRawTemperature();
    
        /**
         * @brief Lee los parámetros de calibración de temperatura desde el sensor.
         */
        void readTemperatureCalibrationData();
    
        /**
         * @brief Compensa la temperatura raw usando los parámetros de calibración.
         *
         * @param raw_temp Valor raw de temperatura.
         * @param t_fine Puntero donde se almacenará el valor t_fine para compensaciones.
         * 
         * @return float Temperatura compensada en °C.
         */
        float compensateTemperature(uint32_t raw_temp, int32_t *t_fine);

        /**
         * @brief Lee los parámetros de calibración de presión desde el sensor.
         */
        void readPressureCalibrationData();
    
        /**
         * @brief Lee el valor raw de presión del sensor.
         *
         * @return uint32_t Valor raw de presión.
         */
        uint32_t readRawPressure();
    
        /**
         * @brief Compensa la presión raw utilizando el valor t_fine.
         *
         * @param raw_press Valor raw de presión.
         * @param t_fine Valor t_fine obtenido en la compensación de temperatura.
         * 
         * @return float Presión compensada en hPa.
         */
        float compensatePressure(uint32_t raw_press, int32_t t_fine);
    
        /**
         * @brief Lee los parámetros de calibración de humedad desde el sensor.
         */
        void readHumidityCalibrationData();
    
        /**
         * @brief Lee el valor raw de humedad del sensor.
         *
         * @param hum_adc Puntero donde se almacenará el valor raw de humedad.
         * @return esp_err_t Resultado de la operación.
         */
        uint16_t readRawHumidity();
    
        /**
         * @brief Compensa la humedad raw utilizando la temperatura compensada.
         *
         * @param hum_adc Valor raw de humedad.
         * @param temp_comp Temperatura compensada en °C.
         * 
         * @return float Humedad compensada en % r.H.
         */
        float compensateHumidity(uint16_t hum_adc, float temp_comp);

        /**
         * @brief Lee los valores de temperatura, presión y humedad compensados.
         *
         * @param data Estructura donde se almacenarán los valores medidos.
         * 
         * @return float Valor de la medida (temperatura, presión o humedad).
         */
        bool getMeasure(measure_t *data);

    };


/* === C++ header ============================================================================== */

#ifdef __cplusplus
extern "C"
{
#endif

    // C compatible function declarations

#ifdef __cplusplus
}
#endif

/* === End of documentation ==================================================================== */
#endif // BME680_H