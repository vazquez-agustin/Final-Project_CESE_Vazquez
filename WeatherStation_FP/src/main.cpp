
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "driver/i2c.h"

#define I2C_MASTER_NUM      I2C_NUM_0      // I2C port number
#define I2C_MASTER_SCL_IO   5         // I2C SCL pin
#define I2C_MASTER_SDA_IO   4         // I2C SDA pin
#define I2C_MASTER_FREQ_HZ  400000    // Frequency (400 kHz - FAST MODE)
#define I2C_SLAVE_ADDR      0x76
// BME680 chip id
#define BME680_CHIP_ID                 0x61    // BME680_REG_ID<7:0>

static const char *TAG = "I2C";

/*

I2C INITIALIZATION FUNCTION

*/

static esp_err_t set_i2c(void)
{
    i2c_config_t i2c_config = {};

    i2c_config.mode = I2C_MODE_MASTER;
    i2c_config.sda_io_num = I2C_MASTER_SDA_IO;
    i2c_config.sda_pullup_en = true;
    i2c_config.scl_io_num = I2C_MASTER_SCL_IO;
    i2c_config.scl_pullup_en = true;
    i2c_config.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_config.clk_flags = 0;

    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &i2c_config));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, ESP_INTR_FLAG_LEVEL1));
    //ESP_ERROR_CHECK hace un handling de error

    return ESP_OK;
}

int i2c_slave_read(uint8_t addr, uint8_t reg, uint8_t *data) {
    if (!data) {
        return ESP_ERR_INVALID_ARG; // Validación de puntero de datos
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true); // 7-bit Address (<<1)
    i2c_master_write_byte(cmd, reg, true); // Registro a leer
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_READ, true); // Dirección y lectura
    i2c_master_read_byte(cmd, data, I2C_MASTER_NACK); // Leer un byte y enviar NACK
    i2c_master_stop(cmd);

    esp_err_t err = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);

    return err; // Devuelve el estado de la operación
}
/*
static esp_err_t read_chip_id(uint8_t *chip_id) {
    
    uint8_t reg_addr = 0xD0;

    esp_err_t ret = i2c_master_write_read_device(I2C_NUM_0,
                                                 I2C_SLAVE_ADDR,
                                                 &reg_addr,
                                                 1,
                                                 chip_id,
                                                 1,
                                                 pdMS_TO_TICKS(1000));

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C Read failed: %s", esp_err_to_name(ret));
    }

    return ret;
}
*/
/*
static void i2c_scanner(void) {
    for (uint8_t i = 1; i < 127; i++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (i << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);
        esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
        i2c_cmd_link_delete(cmd);

        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "Device found at address 0x%02X", i);
        }
    }
}
*/
extern "C" void app_main(void)
{

    uint8_t chip_id = 0;

    ESP_ERROR_CHECK(set_i2c());

    if (i2c_slave_read(I2C_SLAVE_ADDR, 0xD0, &chip_id) == ESP_OK) {
        ESP_LOGI(TAG, "BME680 Chip ID: 0x%02X", chip_id);
        if (chip_id == 0x61) {
            ESP_LOGI(TAG, "Chip ID correcto. El sensor BME680 está funcionando.");
        } else {
            ESP_LOGW(TAG, "Chip ID incorrecto. Esperado: 0x61, obtenido: 0x%02X", chip_id);
        }
    } else {
        ESP_LOGE(TAG, "Error al leer el Chip ID del BME680.");
    }
    
}