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

#ifndef WIND_DIRECTION_H
#define WIND_DIRECTION_H

/** @file    wind_direction.h
 ** @brief   Interfaz pública de la clase WindDirectionSensor
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
 * @class WindDirectionSensor
 *
 * @brief Clase para la lectura de dirección de viento desde una veleta analógica.
 */
class WindDirectionSensor
{
private:
    ADC *adc;

public:
    /**
     * @brief Constructor
     * @param channel Canal ADC utilizado
     */
    WindDirectionSensor(adc1_channel_t channel);

    /**
     * @brief Obtiene el ángulo de dirección del viento (0° a 360°)
     *
     * @return Dirección en grados
     */
    uint16_t getDirection();

    /**
     * @brief Destructor
     */
    ~WindDirectionSensor();
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

#endif // WIND_DIRECTION_H
