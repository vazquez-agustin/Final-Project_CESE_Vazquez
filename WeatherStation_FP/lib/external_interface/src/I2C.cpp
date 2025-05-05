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

/** @file  I2C.cpp
 ** @brief Implementación de la clase I2C.
 **/

/* === Headers files inclusions ================================================================ */
#include "I2C.h"
/* === Macros definitions ====================================================================== */
/** @brief Puerto I2C utilizado. */
#define I2C_MASTER_NUM I2C_NUM_0      
/** @brief Dirección I2C utilizado por el sensor BME680. */   
#define I2C_ADDRESS    0x77 
/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

/**
 * @brief Constructor de la clase I2C.
 *
 * @param _i2c_num Puerto I2C que se utilizará.
 * @param _sda_io Pin que se usará para SDA.
 * @param _scl_io Pin que se usará para SCL.
 * @param _clk_speed Velocidad de reloj del bus I2C (en Hz).
 */
I2C::I2C(i2c_port_t _i2c_num, gpio_num_t _sda_io, gpio_num_t _scl_io, uint32_t _clk_speed)
    : i2c_num(_i2c_num), sda_io(_sda_io), scl_io(_scl_io), clk_speed(_clk_speed)
{
}

/**
 * @brief Destructor de la clase I2C.
 */
I2C::~I2C() {
    i2c_driver_delete(i2c_num);
}

/**
 * @brief Configura e inicializa el bus I2C.
 *
 * @return esp_err_t Resultado de la configuración (ESP_OK si es correcto).
 */
esp_err_t I2C::i2cSetup() {
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = sda_io;
    conf.scl_io_num = scl_io;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = clk_speed;
    
    esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (err != ESP_OK) return err;
    
    return i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER, 0, 0, 0);
}

/**
 * @brief Lee un byte de un registro específico en un dispositivo I2C.
 *
 * @param addr Dirección I2C del dispositivo.
 * @param reg Registro del que se quiere leer.
 * @param data Puntero donde se almacenará el byte leído.
 * 
 * @return esp_err_t Resultado de la operación.
 */
esp_err_t I2C::readRegister(uint8_t addr, uint8_t reg, uint8_t *data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    esp_err_t err;
    
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, data, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    
    err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    
    return err;
}

/**
 * @brief Escribe una secuencia de bytes en un registro específico de un dispositivo I2C.
 *
 * @param addr Dirección I2C del dispositivo.
 * @param reg Registro en el que se desea escribir.
 * @param data Puntero a los datos que se enviarán.
 * @param len Número de bytes a escribir.
 * 
 * @return esp_err_t Resultado de la operación.
 */
esp_err_t I2C::writeRegister(uint8_t addr, uint8_t reg, uint8_t *data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    esp_err_t err;
    
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write(cmd, data, len, true);
    i2c_master_stop(cmd);
    
    err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    

    return err;
}