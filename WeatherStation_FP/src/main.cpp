#include "main.h"

extern "C" void app_main() {
    printf("¡Hola Mundo! ESP32-C3 está funcionando\n");

    ESP_ERROR_CHECK(nvs_flash_init());
    WiFiManager wifi;
    wifi.initialization(WIFI_SSID, WIFI_PASS);

    ESP_LOGI("WiFi", "Esperando conexión WiFi...");

    SensorManager manager;
    //manager.dbConnection();
    manager.sensorsRun();
}
