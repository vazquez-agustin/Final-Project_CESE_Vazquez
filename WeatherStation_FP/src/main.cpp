#include "main.h"

extern "C" void app_main() {
    printf("¡Hola Mundo! ESP32-C3 está funcionando\n");

    ESP_ERROR_CHECK(nvs_flash_init());

    SensorManager manager;

    // 1) Inicializar Wi-Fi
    manager.initWiFi();

    // 2) Iniciar lectura de sensores
    manager.sensorsRun();
}
