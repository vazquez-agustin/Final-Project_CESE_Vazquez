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

/** @file  wind_direction.cpp
 ** @brief Implementación de la clase WindDirectionSensor
 **/

/* === Headers files inclusions ================================================================ */
#include "wind_direction.h"
#include <cmath>
/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */
struct WindDirectionMapping {
    float voltage;
    uint16_t angle;
};

static const WindDirectionMapping directionTable[] = {
    {512, 0},    // N
    {1024, 45},  // NE
    {1536, 90},  // E
    {2048, 135}, // SE
    {2560, 180}, // S
    {3072, 225}, // SW
    {3584, 270}, // W
    {4096, 315}  // NW
};
/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

WindDirectionSensor::WindDirectionSensor(adc1_channel_t channel)
{
    adc = new ADC(channel);
    adc->adcSetup();
}

uint16_t WindDirectionSensor::getDirection()
{
   uint32_t rawValue = adc->readRaw();

   uint16_t bestAngle = 0;

   if (rawValue <= 512) {
       // Si el valor es menor que el primer umbral, asignar 0 grados
       bestAngle = 0;
   } else if (rawValue > 512 && rawValue <= 1024) {
       // Si el valor está entre 512 y 1024, asignar 22.5 grados
       bestAngle = 45;
   } else if (rawValue > 1024 && rawValue <= 1536) {
       // Si el valor está entre 1024 y 1536, asignar 67.5 grados
       bestAngle = 90;
   } else if (rawValue > 1536 && rawValue <= 2048) {
       // Si el valor está entre 1536 y 2048, asignar 112.5 grados
       bestAngle = 135;
   } else if (rawValue > 2048 && rawValue <= 2560) {
       // Si el valor está entre 2048 y 2560, asignar 157.5 grados
       bestAngle = 180;
   } else if (rawValue > 2560 && rawValue <= 3072) {
       // Si el valor está entre 2560 y 3072, asignar 202.5 grados
       bestAngle = 225;
   } else if (rawValue > 3072 && rawValue <= 3584) {
       // Si el valor está entre 3072 y 3584, asignar 247.5 grados
       bestAngle = 270;
   } else if (rawValue > 3584 && rawValue <= 4096) {
       // Si el valor es mayor que el último umbral, asignar 292.5 grados
       bestAngle = 315;
   } else {
       // Si no se encuentra un ángulo adecuado, retornar un valor por defecto
       bestAngle = -1; // o cualquier otro valor que indique error
   }

   return bestAngle;
}

WindDirectionSensor::~WindDirectionSensor()
{
    delete adc;
}
