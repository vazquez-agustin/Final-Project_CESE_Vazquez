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

#ifndef I2C_H
#define I2C_H

/** @file  I2C.h
 ** @brief Interfaz pública de la clase I2C.
 **/

/* === Headers files inclusions ================================================================ */
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
/* === Public macros definitions =============================================================== */

/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/**
 * @brief Clase que encapsula la configuración y operaciones básicas de I2C.
 */
class I2C {
    private:
        i2c_port_t i2c_num;       
        gpio_num_t sda_io;        
        gpio_num_t scl_io;        
        uint32_t clk_speed;         
    
    public:
        /**
         * @brief Constructor de la clase I2C.
         *
         * @param _i2c_num Puerto I2C a utilizar.
         * @param _sda_io Pin de datos (SDA).
         * @param _scl_io Pin de reloj (SCL).
         * @param _clk_speed Velocidad de comunicación I2C.
         */
        I2C(i2c_port_t _i2c_num, gpio_num_t _sda_io, gpio_num_t _scl_io, uint32_t _clk_speed);
    
        /**
         * @brief Destructor de la clase I2C.
         */
        ~I2C();
    
        /**
         * @brief Inicializa la configuración del bus I2C.
         *
         * @return esp_err_t Resultado de la operación.
         */
        esp_err_t i2cSetup();
    
        /**
         * @brief Lee un byte de un registro I2C.
         *
         * @param addr Dirección I2C del dispositivo.
         * @param reg Registro a leer.
         * @param data Puntero donde se almacenará el byte leído.
         * 
         * @return esp_err_t Resultado de la operación.
         */
        esp_err_t readRegister(uint8_t addr, uint8_t reg, uint8_t *data);
    
        /**
         * @brief Escribe datos en un registro I2C.
         *
         * @param addr Dirección I2C del dispositivo.
         * @param reg Registro a escribir.
         * @param data Puntero a los datos a escribir.
         * @param len Cantidad de bytes a escribir.
         * 
         * @return esp_err_t Resultado de la operación.
         */
        esp_err_t writeRegister(uint8_t addr, uint8_t reg, uint8_t *data, size_t len);
    };   



#endif // I2C_H
