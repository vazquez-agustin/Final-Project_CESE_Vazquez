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
/** @brief Definición del voltaje de referencia para la conversión del ADC. */
static const float ADC_REF_VOLTAGE = 5.0f;

static const WindDirectionMapping directionTable[] = {
    {0.4f,   0},   // N
    {1.1f,  45},   // NE
    {1.8f,  90},   // E
    {2.5f, 135},   // SE
    {3.2f, 180},   // S
    {3.9f, 225},   // SW
    {4.6f, 270},   // W
    {5.0f, 315}    // NW
};
/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */
struct WindDirectionMapping {
    float voltage;
    uint16_t angle;
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

   float voltage = (rawValue * ADC_REF_VOLTAGE) / adc->MAX_ADC_VALUE;

   float minDiff = 1000.0f;
   uint16_t bestAngle = 0;

   for (auto &entry : directionTable) {
       float diff = std::fabs(voltage - entry.voltage);
       if (diff < minDiff) {
           minDiff = diff;
           bestAngle = entry.angle;
       }
   }

   return bestAngle;
}

WindDirectionSensor::~WindDirectionSensor()
{
    delete adc;
}
