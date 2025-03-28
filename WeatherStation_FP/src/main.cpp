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
#define BME680_REG_CTRL_HUM  0x72
#define BME680_REG_CTRL_MEAS 0x74

static const char *TAG = "BME680";

// Temperature calibration variables
int16_t T1, T2, T3;

// Pressure calibration variables
uint16_t P1;
int16_t P2, P4, P5, P8, P9;
int8_t P3, P6, P7, P10;

uint8_t H1, H6;
int16_t H2, H4, H5;
int8_t H3, H7;


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
 * @brief Set sensor to Forced Mode (oversampling x1 for temperature and pressure)
 */
void set_forced_mode(void)
{
    uint8_t ctrl_meas = 0x25; // 0b00100101: oversampling x1, forced mode
    i2c_write_register(0x74, &ctrl_meas, 1);
    ESP_LOGI(TAG, "BME680 set to Forced Mode");
}


/**
 * @brief Configura el BME680 para:
 *        - Oversampling x1 en humedad
 *        - Oversampling x2 en temperatura
 *        - Oversampling x16 en presión
 *        - Forced Mode
 */
void forced_mode(void)
{
    // ---------------------------
    // 1) Configurar oversampling de humedad
    //    osrs_h<2:0> = 001 => x1
    // ---------------------------
    uint8_t ctrl_hum = 0x01; // 0b0000_0001
    i2c_write_register(BME680_REG_CTRL_HUM, &ctrl_hum, 1);
    // Nota: según el datasheet, se recomienda escribir ctrl_hum
    // antes de ctrl_meas.

    // ---------------------------
    // 2) Configurar oversampling T y P, y modo forced
    //    - osrs_t<2:0> = 010 => x2
    //    - osrs_p<2:0> = 101 => x16
    //    - mode<1:0>   = 01  => forced mode
    //
    //    Bits en ctrl_meas:
    //      [7:5] = osrs_t = 010
    //      [4:2] = osrs_p = 101
    //      [1:0] = mode   = 01
    //    => 010 101 01 => 0x55
    // ---------------------------
    uint8_t ctrl_meas = 0x55; // 0b0101_0101
    i2c_write_register(BME680_REG_CTRL_MEAS, &ctrl_meas, 1);

    ESP_LOGI("BME680", 
             "set_force_mode: H=1x, T=2x, P=16x, Forced Mode activado (0x72=0x01, 0x74=0x55)");
}

void config_humidity_oversampling()
{
    uint8_t ctrl_hum = 0x01;  // oversampling x1
    i2c_write_register(0x72, &ctrl_hum, 1);
    // Nota: Valores de oversampling según la tabla:
    //  - 0x02 => x2
    //  - 0x03 => x4
    //  - 0x04 => x8
    //  - 0x05 => x16
    ESP_LOGI(TAG, "Humidity oversampling set to x1");
}

/**
 * @brief Reads raw temperature data from BME680
 */
uint32_t read_raw_temperature()
{
    uint8_t temp_msb, temp_lsb, temp_xlsb;
 
    if (i2c_read_register(0x22, &temp_msb)!= ESP_OK) return 0;
    if (i2c_read_register(0x23, &temp_lsb)!= ESP_OK) return 0;
    if (i2c_read_register(0x24, &temp_xlsb)!= ESP_OK) return 0;

    uint32_t raw_temp = ((uint32_t)temp_msb << 12) | ((uint32_t)temp_lsb << 4) | (temp_xlsb >> 4);

    ESP_LOGI(TAG, "Raw Temperature: 0x%05lX (%lu)", (unsigned long)raw_temp, (unsigned long)raw_temp);
    return raw_temp;
}

/**
 * @brief Reads raw pressure data from registers 0x1F, 0x20, 0x21
 */
uint32_t read_raw_pressure(void)
{
    uint8_t press_msb, press_lsb, press_xlsb;
    if (i2c_read_register(0x20, &press_lsb) != ESP_OK) return 0;
    if (i2c_read_register(0x1F, &press_msb) != ESP_OK) return 0;
    if (i2c_read_register(0x21, &press_xlsb) != ESP_OK) return 0;
    
    uint32_t raw_press = ((uint32_t)press_msb << 12) | ((uint32_t)press_lsb << 4) | (press_xlsb >> 4);
    return raw_press;
}

uint16_t read_raw_humidity(void)
{
    uint8_t hum_msb, hum_lsb;
    if (i2c_read_register(0x25, &hum_msb) != ESP_OK) return 0;
    if (i2c_read_register(0x26, &hum_lsb) != ESP_OK) return 0;
    
    return ((uint16_t)hum_msb << 8) | hum_lsb;
}

/**
 * @brief Reads temperature calibration data from BME680
 */
void read_t_calibration_data()
{
    uint8_t calib_data[5];

    i2c_read_register(0xE9, &calib_data[0]); // T1 LSB
    i2c_read_register(0xEA, &calib_data[1]); // T1 MSB
    i2c_read_register(0x8A, &calib_data[2]); // T2 LSB
    i2c_read_register(0x8B, &calib_data[3]); // T2 MSB
    i2c_read_register(0x8C, &calib_data[4]); // T3

    T1 = (calib_data[1] << 8) | calib_data[0];
    T2 = (int16_t)((calib_data[3] << 8) | calib_data[2]);
    T3 = (int8_t)calib_data[4];

    ESP_LOGI(TAG, "Calibration: T1=%d, T2=%d, T3=%d", T1, T2, T3);
}

/**
 * @brief Reads pressure calibration data from the sensor
 */
void read_p_calibration_data()
{
    uint8_t calib[18];

    i2c_read_register(0x8E, &calib[0]);  // P1 LSB
    i2c_read_register(0x8F, &calib[1]);  // P1 MSB
    i2c_read_register(0x90, &calib[2]);  // P2 LSB
    i2c_read_register(0x91, &calib[3]);  // P2 MSB
    i2c_read_register(0x92, &calib[4]);  // P3
    i2c_read_register(0x94, &calib[5]);  // P4 LSB
    i2c_read_register(0x95, &calib[6]);  // P4 MSB
    i2c_read_register(0x96, &calib[7]);  // P5 LSB
    i2c_read_register(0x97, &calib[8]);  // P5 MSB
    i2c_read_register(0x99, &calib[9]);  // P6 
    i2c_read_register(0x98, &calib[10]); // P7 
    i2c_read_register(0x9C, &calib[11]); // P8 LSB
    i2c_read_register(0x9D, &calib[12]); // P8 MSB
    i2c_read_register(0x9E, &calib[13]); // P9 LSB
    i2c_read_register(0x9F, &calib[14]); // P9 MSB
    i2c_read_register(0xA0, &calib[15]); // P10

    // Convert calibration values
    P1 = (uint16_t)((calib[1] << 8) | calib[0]);
    P2 = (int16_t)((calib[3] << 8) | calib[2]);
    P3 = (int8_t)calib[4];
    P4 = (int16_t)((calib[6] << 8) | calib[5]);
    P5 = (int16_t)((calib[8] << 8) | calib[7]);
    P6 = (int8_t)calib[9];
    P7 = (int8_t)calib[10];
    P8 = (int16_t)((calib[12] << 8) | calib[11]);
    P9 = (int16_t)((calib[14] << 8) | calib[13]);
    P10 = (int8_t)calib[15];
    
    ESP_LOGI(TAG, "Pressure Calibration: P1=%u, P2=%d, P3=%d, P4=%d, P5=%d, P6=%d, P7=%d, P8=%d, P9=%d, P10=%d",
             P1, P2, P3, P4, P5, P6, P7, P8, P9, P10);
}

void read_h_calibration_data(void)
{
    uint8_t buf_E2, buf_E7;
    
    // Leer H1: 0xE2 (bits 3:0) y 0xE3 (8 bits)
    uint8_t h1_low, h1_high;
    i2c_read_register(0xE2, &buf_E2);
    h1_low = buf_E2 & 0x0F;  // bits [3:0]
    i2c_read_register(0xE3, &h1_high);
    H1 = (h1_high << 4) | h1_low;
    
    // Leer H2: 0xE1 y 0xE2 (bits 7:4)
    uint8_t h2_high, h2_low;
    i2c_read_register(0xE1, &h2_high);
    // Releer 0xE2 para obtener la parte alta
    //i2c_read_register(0xE2, &buf);
    h2_low = (buf_E2 >> 4) & 0x0F;
    H2 = (int16_t)((h2_high << 4) | h2_low);
    if (H2 > 2047)
        H2 -= 4096;
    
    // Leer H3 a H7 (cada uno en un registro)
    i2c_read_register(0xE4, (uint8_t *)&H3);
    i2c_read_register(0xE5, (uint8_t *)&H4);
    i2c_read_register(0xE6, (uint8_t *)&H5);
    i2c_read_register(0xE7, &buf_E7);
    H6 = buf_E7;
    i2c_read_register(0xE8, (uint8_t *)&H7);
    
    ESP_LOGI(TAG, "Humidity Calibration: H1=%u, H2=%d, H3=%d, H4=%d, H5=%d, H6=%u, H7=%d",
             H1, H2, H3, H4, H5, H6, H7);
}

/**
 * @brief Compensate temperature reading and output the t_fine value.
 *
 * @param raw_temp Raw temperature data.
 * @param t_fine Pointer to store the fine temperature value.
 * @return Compensated temperature in °C.
 */
float compensate_temperature(uint32_t raw_temp, int32_t *t_fine)
{
    float var1, var2, temp;
    
    var1 = ((float)raw_temp / 16384.0f - (float)T1 / 1024.0f) * (float)T2;
    var2 = (((float)raw_temp / 131072.0f - (float)T1 / 8192.0f) *
            ((float)raw_temp / 131072.0f - (float)T1 / 8192.0f)) * ((float)T3 * 16.0f);
    *t_fine = (int32_t)(var1 + var2);
    temp = ((float)*t_fine) / 5120.0f;
    
    return temp;
}

/**
 * @brief Compensate raw pressure data using Bosch's formula.
 *
 * @param raw_press Raw pressure reading.
 * @param t_fine Temperature fine value from temperature compensation.
 * @return Compensated pressure in Hectopascals (hPa).
 */
float compensate_pressure(uint32_t raw_press, int32_t t_fine)
{
    double var1, var2, var3, press_comp;
    
    var1 = ((double)t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double)P6 / 131072.0);
    var2 = var2 + (var1 * (double)P5 * 2.0);
    var2 = (var2 / 4.0) + ((double)P4 * 65536.0);
    var1 = ((((double)P3 * var1 * var1) / 16384.0) + ((double)P2 * var1)) / 524288.0;
    var1 = (1.0 + (var1 / 32768.0)) * (double)P1;
    press_comp = 1048576.0 - (double)raw_press;
    press_comp = ((press_comp - (var2 / 4096.0)) * 6250.0) / var1;
    var1 = ((double)P9 * press_comp * press_comp) / 2147483648.0;
    var2 = press_comp * ((double)P8 / 32768.0);
    var3 = (press_comp / 256.0) * (press_comp / 256.0) * (press_comp / 256.0) * ((double)P10 / 131072.0);
    press_comp = press_comp + (var1 + var2 + var3 + ((double)P7 * 128.0)) / 16.0;
    
    return (float)press_comp / 100.0; // in hPa
}

float compensate_humidity(uint16_t hum_adc, float temp_comp)
{
    float var1, var2, var3, var4, hum_comp;
    
    var1 = hum_adc - (((double)H1 * 16.0) + (((double)H3 / 2.0) * temp_comp));
    var2 = var1 * (((double)H2 / 262144.0) * (1.0 + (((double)H4 / 16384.0) * 
            temp_comp) + (((double)H5 / 1048576.0) * temp_comp * temp_comp)));
    var3 = (double)H6 / 16384.0;
    var4 = (double)H7 / 2097152.0;
    hum_comp = var2 + ((var3 + (var4 * temp_comp)) * var2 * var2);
    
    return hum_comp;  // valor en % r.H.
}


extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Initializing I2C...");
    if (set_i2c() == ESP_OK) {
        ESP_LOGI(TAG, "I2C initialized successfully.");
    } else {
        ESP_LOGE(TAG, "I2C initialization failed.");
        return;
    }
    
    // Read calibration data from the sensor
    read_t_calibration_data();
    read_p_calibration_data();
    read_h_calibration_data();
    
    //uint8_t ctrl_hum = 0x01;  // Oversampling ×1 para humedad
    //i2c_write_register(0x72, &ctrl_hum, 1);
    //ESP_LOGI(TAG, "Humidity oversampling set to x1");

    while (1) {
        forced_mode();
        vTaskDelay(pdMS_TO_TICKS(100));
        
        // Read and compensate temperature to obtain t_fine (required for pressure compensation)
        uint32_t raw_temp = read_raw_temperature();
        int32_t t_fine = 0;
        float temperature = compensate_temperature(raw_temp, &t_fine);
        ESP_LOGI(TAG, "Temperature: %.2f °C", temperature);
        
        // Read raw pressure and compensate using the previously computed t_fine
        uint32_t raw_press = read_raw_pressure();
        float pressure = compensate_pressure(raw_press, t_fine);
        ESP_LOGI(TAG, "Pressure: %.2f hPa", pressure);

        // Read raw pressure and compensate using the previously computed t_fine
        uint32_t raw_hum = read_raw_humidity();
        float humidity = compensate_humidity(raw_hum, temperature);
        ESP_LOGI(TAG, "Humidity: %.2f %% r.H.", humidity);
        
        vTaskDelay(pdMS_TO_TICKS(2000)); // Read every 2 seconds
    }
}
