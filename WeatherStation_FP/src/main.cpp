
#include "main.h"
#include "esp_log.h"
#include <iostream>

// Configuración de pines y frecuencia
#define I2C_MASTER_SDA_IO GPIO_NUM_4    
#define I2C_MASTER_SCL_IO GPIO_NUM_5
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000
#define I2C_ADDRESS 0x76

static const char *TAG = "I2C";

// Prototipos privados
static void i2c_master_init(void);
esp_err_t i2c_read(uint8_t address, uint8_t *data_rd, size_t size);
esp_err_t i2c_write(uint8_t address, uint8_t *data_wr, size_t size);

void i2c_init(void) {
    i2c_master_init();
}

esp_err_t i2c_read(uint8_t address, uint8_t *data_rd, size_t size) {
    return i2c_master_read_from_device(I2C_MASTER_NUM, address, data_rd, size, pdMS_TO_TICKS(1000));
}

esp_err_t i2c_write(uint8_t address, uint8_t *data_wr, size_t size) {
    return i2c_master_write_to_device(I2C_MASTER_NUM, address, data_wr, size, pdMS_TO_TICKS(1000));
}

void i2c_master_init() {
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = 0;

    esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error configurando I2C: %s", esp_err_to_name(err));
        return;
    }
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

uint8_t bme680_read_chip_id() {
    uint8_t chip_id = 0;
    uint8_t reg = 0xD0;
    esp_err_t err;

    // Enviar registro
    err = i2c_write(I2C_ADDRESS, &reg, 1);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error escribiendo a I2C: %s", esp_err_to_name(err));
        return 0xFF;
    }

    // Leer el valor
    err = i2c_read(I2C_ADDRESS, &chip_id, 1);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error leyendo de I2C: %s", esp_err_to_name(err));
        return 0xFF;
    }

    return chip_id;
}

extern "C" void app_main() {
    i2c_init(); // Inicialización I2C
    uint8_t chip_id = bme680_read_chip_id();

    if (chip_id == 0xFF) {
        ESP_LOGE(TAG, "No se pudo leer el ID del chip");
    } else {
        std::cout << "Chip ID leído: 0x" << std::hex << static_cast<int>(chip_id) 
                  << ", ID esperado: 0x61" << std::endl;
    }
}