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

#ifndef BME680_DEFINES_H
#define BME680_DEFINES_H

/** @file  BME680.h
 ** @brief Interfaz pública de la clase BME680.
 **/

/* === Headers files inclusions ================================================================ */

/* === Public macros definitions =============================================================== */

/** @brief Puerto I2C. */
// #define I2C_MASTER_NUM        I2C_NUM_0           
/** @brief GPIO pin SDA utilizado por I2C. */
// #define I2C_MASTER_SDA_IO     GPIO_NUM_8
/** @brief GPIO pin SCL utilizado por I2C. */         
// #define I2C_MASTER_SCL_IO     GPIO_NUM_9
/** @brief Frecuencia del reloj I2C. */            
// #define I2C_MASTER_FREQ_HZ    400000
/** @brief Dirección I2C del sensor BME680. */       
#define I2C_REG_ADDRESS       0x77 
/** @brief Registro para control de humedad en el sensor BME680. */
#define BME680_REG_CTRL_HUM   0x72
/** @brief Registro para control de medición en el sensor BME680. */
#define BME680_REG_CTRL_MEAS  0x74
/** @brief Dirección de registro utilizado por el sensor BME680 para activar el modo forzado. */
#define BME680_MODE_FORCED    0x01

/** @brief Dirección de registro del byte más significativo de T1 en el sensor BME680. */
#define BME680_REG_T1_MSB     0xE9
/** @brief Dirección de registro del byte menos significativo de T1 en el sensor BME680. */
#define BME680_REG_T1_LSB     0xEA
/** @brief Dirección de registro del byte más significativo de T2 en el sensor BME680. */
#define BME680_REG_T2_MSB     0x8A
/** @brief Dirección de registro del byte menos significativo de T2 en el sensor BME680. */
#define BME680_REG_T2_LSB     0x8B
/** @brief Dirección de registro del byte T3 en el sensor BME680. */
#define BME680_REG_T3         0x8C
/** @brief Dirección de registro del byte menos significativo (XLSB) de temperatura cruda en el sensor BME680. */
#define BME680_REG_TRAW_XLSB  0x24
/** @brief Dirección de registro del byte menos significativo (LSB) de temperatura cruda en el sensor BME680. */
#define BME680_REG_TRAW_LSB   0x23
/** @brief Dirección de registro del byte más significativo (MSB) de temperatura cruda en el sensor BME680. */
#define BME680_REG_TRAW_MSB   0x22

/** @brief Dirección de registro del byte más significativo de P1 en el sensor BME680. */
#define BME680_REG_P1_MSB     0x8F
/** @brief Dirección de registro del byte menos significativo de P1 en el sensor BME680. */
#define BME680_REG_P1_LSB     0x8E
/** @brief Dirección de registro del byte más significativo de P2 en el sensor BME680. */
#define BME680_REG_P2_MSB     0x91
/** @brief Dirección de registro del byte menos significativo de P2 en el sensor BME680. */
#define BME680_REG_P2_LSB     0x90
/** @brief Dirección de registro del byte P3 en el sensor BME680. */
#define BME680_REG_P3         0x92
/** @brief Dirección de registro del byte más significativo de P4 en el sensor BME680. */
#define BME680_REG_P4_MSB     0x95
/** @brief Dirección de registro del byte menos significativo de P4 en el sensor BME680. */
#define BME680_REG_P4_LSB     0x94
/** @brief Dirección de registro del byte más significativo de P5 en el sensor BME680. */
#define BME680_REG_P5_MSB     0x97
/** @brief Dirección de registro del byte menos significativo de P5 en el sensor BME680. */
#define BME680_REG_P5_LSB     0x96
/** @brief Dirección de registro del byte P6 en el sensor BME680. */
#define BME680_REG_P6         0x99
/** @brief Dirección de registro del byte P7 en el sensor BME680. */
#define BME680_REG_P7         0x98
/** @brief Dirección de registro del byte menos significativo de P8 en el sensor BME680. */
#define BME680_REG_P8_MSB     0x9D
/** @brief Dirección de registro del byte menos significativo de P8 en el sensor BME680. */
#define BME680_REG_P8_LSB     0x9C
/** @brief Dirección de registro del byte menos significativo de P9 en el sensor BME680. */
#define BME680_REG_P9_MSB     0x9F
/** @brief Dirección de registro del byte menos significativo de P9 en el sensor BME680. */
#define BME680_REG_P9_LSB     0x9E
/** @brief Dirección de registro del byte P10 en el sensor BME680. */
#define BME680_REG_P10        0xA0
/** @brief Dirección de registro del byte menos significativo (XLSB) de presión cruda en el sensor BME680. */
#define BME680_REG_PRAW_XLSB  0x21
/** @brief Dirección de registro del byte menos significativo (LSB) de presión cruda en el sensor BME680. */
#define BME680_REG_PRAW_LSB   0x20
/** @brief Dirección de registro del byte más significativo (MSB) de presión cruda en el sensor BME680. */
#define BME680_REG_PRAW_MSB   0x1F

/** @brief Dirección de registro del byte más significativo de H1 en el sensor BME680. */
#define BME680_REG_H1_MSB     0xE3
/** @brief Dirección de registro del byte menos significativo de H1 en el sensor BME680. */
#define BME680_REG_H1_LSB     0xE2
/** @brief Dirección de registro del byte más significativo de H2 en el sensor BME680. */
#define BME680_REG_H2_MSB     0xE1
/** @brief Dirección de registro del byte menos significativo de H2 en el sensor BME680. */
#define BME680_REG_H2_LSB     0xE2
/** @brief Dirección de registro del byte H3 en el sensor BME680. */
#define BME680_REG_H3         0xE4
/** @brief Dirección de registro del byte H4 en el sensor BME680. */
#define BME680_REG_H4         0xE5
/** @brief Dirección de registro del byte H5 en el sensor BME680. */
#define BME680_REG_H5         0xE6
/** @brief Dirección de registro del byte H6 en el sensor BME680. */
#define BME680_REG_H6         0xE7
/** @brief Dirección de registro del byte H7 en el sensor BME680. */
#define BME680_REG_H7         0xE8
/** @brief Dirección de registro del byte menos significativo (LSB) de humedad cruda en el sensor BME680. */
#define BME680_REG_HRAW_MSB   0x25
/** @brief Dirección de registro del byte más significativo (MSB) de humedad cruda en el sensor BME680. */
#define BME680_REG_HRAW_LSB   0x26

/** @brief Dirección de registro del byte más significativo (MSB) de humedad cruda en el sensor BME680. */
#define BME680_OSR_SKIPPED    0x00
/** @brief Dirección de registro del oversampling de 1x para el sensor BME680. */
#define BME680_OSR_1X         0x01
/** @brief Dirección de registro del oversampling de 2x para el sensor BME680. */
#define BME680_OSR_2X         0x02
/** @brief Dirección de registro del oversampling de 4x para el sensor BME680. */
#define BME680_OSR_4X         0x03
/** @brief Dirección de registro del oversampling de 8x para el sensor BME680. */
#define BME680_OSR_8X         0x04
/** @brief Dirección de registro del oversampling de 16x para el sensor BME680. */
#define BME680_OSR_16X        0x05

// Dirección I2C del sensor BME680
#define BME680_I2C_ADDR       0x77

// Registros para lectura cruda de presión (20 bits: MSB, LSB, XLSB)
#define REG_PRESS_MSB     0x1F
#define REG_PRESS_LSB     0x20
#define REG_PRESS_XLSB    0x21

// Registros para lectura cruda de temperatura (20 bits: MSB, LSB, XLSB)
#define REG_TEMP_MSB      0x22
#define REG_TEMP_LSB      0x23
#define REG_TEMP_XLSB     0x24

// Registros para lectura cruda de humedad (16 bits: MSB, LSB)
#define REG_HUM_MSB       0x25
#define REG_HUM_LSB       0x26

// Registro donde se lee el ID del chip (para validar presencia)
#define REG_CHIP_ID       0xD0
#define BME680_CHIP_ID    0x61    // Valor esperado del chip ID

/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/* === End of documentation ==================================================================== */
#endif // BME680_DEFINES_H