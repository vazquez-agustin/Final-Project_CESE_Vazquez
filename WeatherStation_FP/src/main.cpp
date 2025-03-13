#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include "driver/i2c.h"

// I2C Configuration
#define I2C_MASTER_NUM      I2C_NUM_0   // I2C port
#define I2C_MASTER_SDA_IO   GPIO_NUM_8  // SDA pin
#define I2C_MASTER_SCL_IO   GPIO_NUM_9  // SCL pin
#define I2C_MASTER_FREQ_HZ  400000      // I2C clock speed
#define BME680_I2C_ADDR     0x77        // BME680 I2C Address

static const char *TAG = "BME680";

// Calibration coefficients
int16_t T1, T2, T3;

/**
 * @brief Initialize I2C master
 */
static esp_err_t set_i2c(void)
{
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;

    esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (err != ESP_OK) return err;

    return i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER, 0, 0, 0);
}

/**
 * @brief Read a single byte from a BME680 register
 */
esp_err_t i2c_read_register(uint8_t reg_addr, uint8_t *data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    esp_err_t err;

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BME680_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BME680_I2C_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, data, I2C_MASTER_NACK);
    i2c_master_stop(cmd);

    err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);

    return err;
}

/**
 * @brief Write data to a BME680 register
 */
esp_err_t i2c_write_register(uint8_t reg_addr, uint8_t *data, size_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    esp_err_t err;

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BME680_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_write(cmd, data, len, true);
    i2c_master_stop(cmd);

    err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C write failed with error: %s", esp_err_to_name(err));
    }
    return err;
}

/**
 * @brief Configura el sensor en modo FORZADO (Forced Mode)
 */
void set_forced_mode()
{
    uint8_t ctrl_meas = 0b00100101; // Oversampling x1, Forced Mode
    i2c_write_register(0x74, &ctrl_meas, 1);
    ESP_LOGI(TAG, "BME680 set to Forced Mode");
}

/**
 * @brief Reads raw temperature data from BME680
 */
uint32_t read_raw_temperature()
{
    uint8_t temp_msb, temp_lsb, temp_xlsb;
    esp_err_t err;

    err = i2c_read_register(0x22, &temp_msb);
    if (err != ESP_OK) return 0;
    
    err = i2c_read_register(0x23, &temp_lsb);
    if (err != ESP_OK) return 0;
    
    err = i2c_read_register(0x24, &temp_xlsb);
    if (err != ESP_OK) return 0;

    uint32_t raw_temp = ((uint32_t)temp_msb << 12) | ((uint32_t)temp_lsb << 4) | (temp_xlsb >> 4);

    ESP_LOGI(TAG, "Raw Temperature: 0x%05lX (%lu)", (unsigned long)raw_temp, (unsigned long)raw_temp);
    return raw_temp;
}

/**
 * @brief Reads temperature calibration data from BME680
 */
void read_calibration_data()
{
    uint8_t calib_data[5];

    i2c_read_register(0x89, &calib_data[0]); // T1 LSB
    i2c_read_register(0x8A, &calib_data[1]); // T1 MSB
    i2c_read_register(0x8B, &calib_data[2]); // T2 LSB
    i2c_read_register(0x8C, &calib_data[3]); // T2 MSB
    i2c_read_register(0x8D, &calib_data[4]); // T3

    T1 = (calib_data[1] << 8) | calib_data[0];
    T2 = (int16_t)((calib_data[3] << 8) | calib_data[2]);
    T3 = (int8_t)calib_data[4];

    ESP_LOGI(TAG, "Calibration: T1=%d, T2=%d, T3=%d", T1, T2, T3);
}

/**
 * @brief Converts raw temperature to °C
 */
float compensate_temperature(uint32_t raw_temp)
{
    float var1, var2, temp_comp;

    var1 = (((float)raw_temp / 16384.0) - ((float)T1 / 1024.0)) * ((float)T2);
    var2 = ((((float)raw_temp / 131072.0) - ((float)T1 / 8192.0)) *
           (((float)raw_temp / 131072.0) - ((float)T1 / 8192.0))) *
           ((float)T3);

    temp_comp = (var1 + var2) / 5120.0;
    return temp_comp;
}






extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Initializing I2C...");
    if (set_i2c() != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C initialization failed.");
        return;
    }

    read_calibration_data(); // Read T1, T2, T3

    while (1)
    {

        set_forced_mode(); // Activar el modo antes de cada lectura
        vTaskDelay(pdMS_TO_TICKS(100)); // 🔹 Esperar un poco antes de leer

        uint32_t raw_temp = read_raw_temperature();
        float temperature = compensate_temperature(raw_temp);

        ESP_LOGI(TAG, "Temperature: %.2f °C", temperature);

        vTaskDelay(pdMS_TO_TICKS(2000)); // Read every 2 seconds
    }

}
