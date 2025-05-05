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

#ifndef WIFI_H
#define WIFI_H

/** @file  WiFi.h
 ** @brief Clase encargada de gestionar la conexión Wi-Fi del dispositivo.
 **/

/* === Headers files inclusions ================================================================ */
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <string.h>
#include <string>
/* === Public macros definitions =============================================================== */

/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/**
 * @class WiFiManager
 * 
 * @brief Clase encargada de gestionar la conexión Wi-Fi del dispositivo.
 */
class WiFiManager
{
private:
    /** @brief Estado actual de la conexión Wi-Fi. */
    bool connected = false;
    /**
     * @brief Manejador estático de eventos Wi-Fi del sistema.
     *
     * @param arg Argumentos pasados al manejador.
     * @param event_base Base del evento (Wi-Fi o IP).
     * @param event_id ID específico del evento generado.
     * @param event_data Datos adicionales sobre el evento.
     */
    static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
    /**
     * @brief Método para manejar los eventos Wi-Fi recibidos.
     *
     * @param event_base Base del evento.
     * @param event_id ID específico del evento.
     * @param event_data Datos adicionales sobre el evento.
     */
    void handle_event(esp_event_base_t event_base, int32_t event_id, void *event_data);

public:
    /**
     * @brief Constructor por defecto de WiFiManager.
     */
    WiFiManager();
    /**
     * @brief Inicializa y conecta el dispositivo a una red Wi-Fi especificada.
     *
     * @param ssid SSID (nombre) de la red Wi-Fi.
     * @param password Contraseña de la red Wi-Fi.
     */
    void initialization(std::string ssid, std::string password);
    /**
     * @brief Consulta el estado actual de la conexión Wi-Fi.
     *
     * @return true si está conectado; false en caso contrario.
     */
    bool isConnected() const { return connected; }
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
#endif // WIFI_H