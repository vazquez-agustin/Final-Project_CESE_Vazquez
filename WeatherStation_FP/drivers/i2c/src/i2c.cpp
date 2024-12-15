/*!
    Author name: Agustín Jesús Vazquez
    Creation date; 12/12/2024
*/

#include "driver/i2c.h"
#include "../drivers/i2c/inc/i2c.h"

// private definitions
#define I2C_MASTER_SDA_IO GPIO_NUM_4
#define I2C_MASTER_SCL_IO GPIO_NUM_5
#define I2C_MASTER_NUM I2C_NUM_0     // first i2c controller available in ESP32
#define I2C_MASTER_FREQ_HZ 100000

static const char *TAG = "I2C";

// Private function prototypes
static void i2c_master_init(void);
//static void i2c_set_register(uint8_t reg, uint8_t value);
//static uint8_t i2c_get_register(uint8_t reg);

// Public function implementations
void i2c_init(void) {
    i2c_master_init();
}

esp_err_t i2c_read(uint8_t address, uint8_t *data_rd, size_t size) {
    return i2c_master_read_from_device(I2C_MASTER_NUM, address, data_rd, size, pdMS_TO_TICKS(1000));
}

esp_err_t i2c_write(uint8_t address, uint8_t *data_wr, size_t size) {
    return i2c_master_write_to_device(I2C_MASTER_NUM, address, data_wr, size, pdMS_TO_TICKS(1000));
}

// Private function implementations
void i2c_master_init () {
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ; // define frequency
    conf.clk_flags = 0;

    esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "i2c_param_config failed");
        return;
    }
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}