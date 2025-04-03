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

#ifndef INFLUXDB_H
#define INFLUXDB_H

/** @file    influxdb.h
 ** @brief   Interfaz pública de la clase InfluxDBClient
 **/

/* === Headers files inclusions ================================================================ */

#include "parameters.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include <cstring>
#include <string>

/* === Public macros definitions =============================================================== */

/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/**
 * @class InfluxDBClient
 *
 * @brief Clase encargada de gestionar la conexión y envío de datos hacia InfluxDB.
 */
class InfluxDBClient
{
private:
    /**
     * @brief Manejador de eventos HTTP para el cliente InfluxDB.
     *
     * @param evt Puntero a la estructura del evento HTTP recibido.
     *
     * @return Código de estado ESP_OK tras procesar el evento.
     */
    static esp_err_t http_event_handler(esp_http_client_event_t *evt);

public:
    /**
     * @brief Envía datos medidos de los sensores hacia InfluxDB.
     *
     * @param moisture_value Valor actual de humedad del suelo (en %).
     * @param speed_value Valor actual de la velocidad del viento (en m/s).
     */
    void send_data(uint32_t moisture_value, uint32_t speed_value, float temperature_value, float pressure_value, float humidity_value);
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
#endif // INFLUXDB_H
