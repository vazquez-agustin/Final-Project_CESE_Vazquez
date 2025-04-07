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

#ifndef SOIL_MOISTURE_H
#define SOIL_MOISTURE_H

/** @file  soil_moisture.h
 ** @brief Interfaz pública de la clase SoilMoistureSensor.
 **/

/* === Headers files inclusions ================================================================ */
#include "ADC.h"
#include "driver/adc.h"
#include <stdint.h>
/* === Public macros definitions =============================================================== */

/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/**
 * @class SoilMoistureSensor
 * 
 * @brief Clase encargada de medir la humedad del suelo mediante lecturas analógicas del ADC.
 */
class SoilMoistureSensor
{
private:
    /** @brief Instancia del ADC utilizada para realizar las lecturas del sensor. */
    ADC *adc;

public:
    /**
     * @brief Constructor de la clase.
     * 
     * @param channel Canal ADC utilizado para la medición del sensor.
     */
    SoilMoistureSensor(adc1_channel_t channel);
    /**
     * @brief Obtiene la humedad actual del suelo como porcentaje.
     * 
     * @return Humedad del suelo expresada en porcentaje (0-100%).
     */
    uint32_t readPercentage();
    /**
     * @brief Destructor de la clase.
     */
    ~SoilMoistureSensor();
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
#endif // SOIL_MOISTURE_H