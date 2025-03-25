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

#ifndef ADC_H
#define ADC_H

/** @file  ADC.h
 ** @brief Interfaz pública de la clase ADC.
 **/

/* === Headers files inclusions ================================================================ */

#include "driver/adc.h"
#include "esp_adc_cal.h"

/* === Public macros definitions =============================================================== */

/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/**
 * @class ADC
 * 
 * @brief Clase que encapsula la configuración y manejo de lecturas mediante el conversor analógico-digital (ADC).
 */
class ADC {
private:
    /** @brief Canal ADC utilizado. */
    adc1_channel_t channel;
    /** @brief Unidad ADC utilizada (generalmente ADC_UNIT_1). */
    adc_unit_t unit;
    /** @brief Atenuación utilizada para ajustar el rango de voltaje. */
    adc_atten_t attenuation;
    /** @brief Características del ADC para calibración. */
    esp_adc_cal_characteristics_t *adc_chars;

public:
    /** @brief Valor máximo que puede tomar el ADC (12 bits). */
    const int MAX_ADC_VALUE = 4095;
    /** @brief Voltaje de referencia del ADC (mV). */
    const int REF_VOLTAGE = 1100;

    /**
     * @brief Constructor de la clase ADC.
     *
     * @param _channel Canal ADC a utilizar.
     * @param _unit Unidad ADC a utilizar (por defecto ADC_UNIT_1).
     * @param _attenuation Atenuación para ajustar el rango de entrada (por defecto ADC_ATTEN_DB_12).
     * @param _adc_chars Características ADC opcionales para calibración.
     */
    ADC(adc1_channel_t _channel, adc_unit_t _unit = ADC_UNIT_1, adc_atten_t _attenuation = ADC_ATTEN_DB_12, esp_adc_cal_characteristics_t *_adc_chars = nullptr);
    ~ADC();
    
    /**
     * @brief Configura el canal ADC según los parámetros definidos.
     */
    void adcSetup();

    /**
     * @brief Realiza una lectura directa del valor raw del ADC.
     *
     * @return Valor ADC crudo (raw).
     */
    uint32_t readRaw();
};

#endif // ADC_H
