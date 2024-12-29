
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"

#include "driver/i2c.h"
#include "driver/gpio.h"

#define I2C_MASTER_NUM      I2C_NUM_0       // I2C port number
#define I2C_MASTER_SCL_IO   GPIO_NUM_5      // I2C SCL pin
#define I2C_MASTER_SDA_IO   GPIO_NUM_4      // I2C SDA pin
#define I2C_MASTER_FREQ_HZ  400000          // Frequency (100 kHz - FAST MODE)

// Registers
#define BME680_I2C_ADDR 0x77               // Slave Address (device)
#define BME680_CHIP_ID_REG 0xD0            // Chip ID Address (register)
#define EXPECTED_CHIP_ID 0x61              // Expected Chip ID

static const char *TAG = "BME680";

/*

I2C INITIALIZATION FUNCTION

*/

static esp_err_t set_i2c(void)
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    
    i2c_param_config(I2C_NUM_0, &conf);
    if (i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install I2C driver");
        return ESP_FAIL;
    }

    return ESP_OK;
}

/*

I2C READ FUNCTION

*/

esp_err_t i2c_read_register(uint8_t device_addr, uint8_t reg_addr, uint8_t *data) 
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    esp_err_t err;

    // Start transmission
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);

    // Restart and request data
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, data, I2C_MASTER_NACK);
    i2c_master_stop(cmd);

    err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C read failed with error: %s", esp_err_to_name(err));
    }
    i2c_cmd_link_delete(cmd);
    return err;
}

extern "C" void app_main(void)
{
ESP_LOGI(TAG, "Initializing I2C...");
    esp_err_t err = set_i2c();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error initializing I2C: %s", esp_err_to_name(err));
        return;
    }

    uint8_t chip_id;
    ESP_LOGI(TAG, "Reading the Chip ID...");
    err = i2c_read_register(BME680_I2C_ADDR, BME680_CHIP_ID_REG, &chip_id);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Chip ID read: 0x%02X", chip_id);
        if (chip_id == EXPECTED_CHIP_ID) {
            ESP_LOGI(TAG, "BME680 connected successfully! Chip ID: 0x%02X", chip_id);
        } else {
            ESP_LOGE(TAG, "Incorrect Chip ID: 0x%02X (expected: 0x%02X)", chip_id, EXPECTED_CHIP_ID);
        }
    } else {
        ESP_LOGE(TAG, "Error reading Chip ID: %s", esp_err_to_name(err));
    }
}