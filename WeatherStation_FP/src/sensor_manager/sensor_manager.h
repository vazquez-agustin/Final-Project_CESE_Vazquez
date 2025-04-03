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

#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

/** @file    sensor_manager.h
 ** @brief   Interfaz pública de la clase SoilMoistureSensor.
 **/

/* === Headers files inclusions ================================================================ */
#include "parameters.h"
#include "WiFi.h"
#include "influxdb.h"
#include "soil_moisture.h"
#include "wind_speed.h"
#include "BME680.h"
/* === Public macros definitions =============================================================== */

/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/**
 * @class SensorManager
 * 
 * @brief Gestiona la inicialización, ejecución y transmisión de datos de los sensores.
 */
class SensorManager {
private:
    /** @brief Instancia del administrador de la conexión Wi-Fi del sistema. */
    WiFiManager WiFi;
    /** @brief Instancia del cliente para conexión con la base de datos InfluxDB. */
    InfluxDBClient influxClient;
    /** @brief Instancia del sensor de humedad del suelo. */
    SoilMoistureSensor soilMoistureSensor;
    /** @brief Instancia del sensor de velocidad de viento. */
    WindSpeedSensor windSpeedSensor;

public:
    /**
     * @brief Constructor por defecto de SensorManager.
     */
    SensorManager();
    /**
     * @brief Ejecuta la lectura de los sensores y envía los datos obtenidos a la base de datos.
     */
    void sensorsRun();
    /**
     * @brief Inicializa la conexión Wi-Fi.
     */
    void initWiFi();
    /**
     * @brief Inicializa la conexión con la base de datos InfluxDB.
     */
    void initInfluxDB();
};

/* === C++ header ============================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

// C compatible function declarations

#ifdef __cplusplus
}
#endif

/* === End of documentation ==================================================================== */
#endif // SENSOR_MANAGER_H