#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ============================
// Configuración I2C y Definiciones
// ============================

// I2C Configuration
#define I2C_MASTER_NUM      I2C_NUM_0           // Puerto I2C
#define I2C_MASTER_SDA_IO   GPIO_NUM_8          // Pin SDA (ajusta según tu hardware)
#define I2C_MASTER_SCL_IO   GPIO_NUM_9          // Pin SCL (ajusta según tu hardware)
#define I2C_MASTER_FREQ_HZ  400000              // Frecuencia de reloj I2C (400 kHz)

// Dirección I2C del sensor (ajusta según tu módulo, 0x77 o 0x76)
#define BME680_I2C_ADDR     0x77

// Registros de control y datos (según datasheet)
#define BME680_REG_CTRL_HUM   0x72  // Control de humedad
#define BME680_REG_CTRL_MEAS  0x74  // Control de temperatura, presión y modo
#define BME680_REG_HUM_MSB    0x25  // Humedad MSB
#define BME680_REG_HUM_LSB    0x26  // Humedad LSB

// Definiciones de oversampling
#define BME680_OSR_SKIPPED    0x00
#define BME680_OSR_1X         0x01
#define BME680_OSR_2X         0x02
#define BME680_OSR_4X         0x03
#define BME680_OSR_8X         0x04
#define BME680_OSR_16X        0x05

// Modo forced: bits [1:0] = 0b01
#define BME680_MODE_FORCED    0x01

static const char *TAG = "BME680_HUMIDITY";

// ============================
// Variables de calibración
// ============================

// Humedad
uint8_t par_h6;
uint16_t par_h1;
int16_t par_h2;
int8_t par_h3, par_h4, par_h5, par_h7;

// Temperatura
uint16_t T1;
int16_t T2;
int8_t T3;

// Presión (según datasheet)
uint16_t par_p1;
int16_t par_p2;
int8_t par_p3;
int16_t par_p4;
int16_t par_p5;
int8_t par_p6;
int8_t par_p7;
int16_t par_p8;
int16_t par_p9;
uint8_t par_p10;

// ============================
// Funciones I2C
// ============================
esp_err_t set_i2c(void)
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
        ESP_LOGE(TAG, "I2C write failed: %s", esp_err_to_name(err));
    }
    return err;
}

// ============================
// Configuración del Sensor
// ============================
esp_err_t bme680_configForcedMode(uint8_t humOSR, uint8_t tempOSR, uint8_t presOSR)
{
    esp_err_t err;
    uint8_t data;
    
    // Configurar oversampling de humedad
    err = i2c_read_register(BME680_REG_CTRL_HUM, &data);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error leyendo ctrl_hum");
        return err;
    }
    data &= 0xF8; // Limpiar bits [2:0]
    data |= (humOSR & 0x07);
    err = i2c_write_register(BME680_REG_CTRL_HUM, &data, 1);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error escribiendo ctrl_hum");
        return err;
    }
    
    // Configurar oversampling de temperatura y presión y modo forced
    data = (((tempOSR & 0x07) << 5) | ((presOSR & 0x07) << 2) | BME680_MODE_FORCED);
    err = i2c_write_register(BME680_REG_CTRL_MEAS, &data, 1);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error escribiendo ctrl_meas");
        return err;
    }
    return ESP_OK;
}

// ============================
// Funciones de Temperatura
// ============================
uint32_t read_raw_temperature(void)
{
    uint8_t temp_msb, temp_lsb, temp_xlsb;
    
    if (i2c_read_register(0x22, &temp_msb) != ESP_OK) return 0;
    if (i2c_read_register(0x23, &temp_lsb) != ESP_OK) return 0;
    if (i2c_read_register(0x24, &temp_xlsb) != ESP_OK) return 0;
    
    uint32_t raw_temp = ((uint32_t)temp_msb << 12) | ((uint32_t)temp_lsb << 4) | (temp_xlsb >> 4);
    ESP_LOGI(TAG, "Raw Temperature: 0x%05lX (%lu)", (unsigned long)raw_temp, (unsigned long)raw_temp);
    return raw_temp;
}

void read_t_calibration_data(void)
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
    
    ESP_LOGI(TAG, "Calibración Temperatura: T1=%d, T2=%d, T3=%d", T1, T2, T3);
}

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

// ============================
// Funciones de Presión
// ============================
void read_p_calibration_data(void)
{
    uint8_t p1_lsb, p1_msb;
    i2c_read_register(0x8E, &p1_lsb);
    i2c_read_register(0x8F, &p1_msb);
    par_p1 = (p1_msb << 8) | p1_lsb;
    
    uint8_t p2_lsb, p2_msb;
    i2c_read_register(0x90, &p2_lsb);
    i2c_read_register(0x91, &p2_msb);
    par_p2 = (int16_t)((p2_msb << 8) | p2_lsb);
    
    i2c_read_register(0x92, (uint8_t *)&par_p3); // par_p3 es int8_t
    
    uint8_t p4_lsb, p4_msb;
    i2c_read_register(0x94, &p4_lsb);
    i2c_read_register(0x95, &p4_msb);
    par_p4 = (int16_t)((p4_msb << 8) | p4_lsb);
    
    uint8_t p5_lsb, p5_msb;
    i2c_read_register(0x96, &p5_lsb);
    i2c_read_register(0x97, &p5_msb);
    par_p5 = (int16_t)((p5_msb << 8) | p5_lsb);
    
    i2c_read_register(0x99, (uint8_t *)&par_p6); // par_p6 es int8_t
    i2c_read_register(0x98, (uint8_t *)&par_p7); // par_p7 es int8_t
    
    uint8_t p8_lsb, p8_msb;
    i2c_read_register(0x9C, &p8_lsb);
    i2c_read_register(0x9D, &p8_msb);
    par_p8 = (int16_t)((p8_msb << 8) | p8_lsb);
    
    uint8_t p9_lsb, p9_msb;
    i2c_read_register(0x9E, &p9_lsb);
    i2c_read_register(0x9F, &p9_msb);
    par_p9 = (int16_t)((p9_msb << 8) | p9_lsb);
    
    i2c_read_register(0xA0, &par_p10); // par_p10 es uint8_t
    
    ESP_LOGI(TAG, "Calibración Presión: P1=%u, P2=%d, P3=%d, P4=%d, P5=%d, P6=%d, P7=%d, P8=%d, P9=%d, P10=%u",
             par_p1, par_p2, par_p3, par_p4, par_p5, par_p6, par_p7, par_p8, par_p9, par_p10);
}

uint32_t read_raw_pressure(void)
{
    uint8_t msb, lsb, xlsb;
    if(i2c_read_register(0x1F, &msb) != ESP_OK) return 0;
    if(i2c_read_register(0x20, &lsb) != ESP_OK) return 0;
    if(i2c_read_register(0x21, &xlsb) != ESP_OK) return 0;
    uint32_t raw_press = ((uint32_t)msb << 12) | ((uint32_t)lsb << 4) | (xlsb >> 4);
    ESP_LOGI(TAG, "Raw Pressure: 0x%05lX (%lu)", (unsigned long)raw_press, (unsigned long)raw_press);
    return raw_press;
}

float compensate_pressure(uint32_t raw_press, int32_t t_fine)
{
    double var1, var2, var3, press_comp;
    var1 = ((double)t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double)par_p6 / 131072.0);
    var2 = var2 + (var1 * (double)par_p5 * 2.0);
    var2 = (var2 / 4.0) + ((double)par_p4 * 65536.0);
    var1 = ((((double)par_p3 * var1 * var1) / 16384.0) + ((double)par_p2 * var1)) / 524288.0;
    var1 = (1.0 + (var1 / 32768.0)) * (double)par_p1;
    press_comp = 1048576.0 - (double)raw_press;
    press_comp = ((press_comp - (var2 / 4096.0)) * 6250.0) / var1;
    var1 = ((double)par_p9 * press_comp * press_comp) / 2147483648.0;
    var2 = press_comp * ((double)par_p8 / 32768.0);
    var3 = (press_comp / 256.0) * (press_comp / 256.0) * (press_comp / 256.0) * ((double)par_p10 / 131072.0);
    press_comp = press_comp + (var1 + var2 + var3 + ((double)par_p7 * 128.0)) / 16.0;
    // Convertir a hPa (dividir entre 100)
    return (float)press_comp / 100.0;
}

// ============================
// Funciones de Humedad
// ============================
esp_err_t read_humidity_raw(uint16_t *hum_adc)
{
    uint8_t msb, lsb;
    esp_err_t err;
    
    err = i2c_read_register(BME680_REG_HUM_MSB, &msb);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error leyendo MSB de humedad");
        return err;
    }
    
    err = i2c_read_register(BME680_REG_HUM_LSB, &lsb);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error leyendo LSB de humedad");
        return err;
    }
    
    *hum_adc = ((uint16_t)msb << 8) | lsb;
    ESP_LOGI(TAG, "Valor de humedad raw: %u", *hum_adc);
    
    return ESP_OK;
}

void read_h_calibration_data(void)
{
    uint8_t buf_E2, buf_E7;
    uint8_t h1_low, h1_high;
    uint8_t h2_high, h2_low;
    
    // Leer H1: 0xE2 y 0xE3
    i2c_read_register(0xE2, &buf_E2);
    h1_low = buf_E2 & 0x0F;  // bits [3:0]
    i2c_read_register(0xE3, &h1_high);
    par_h1 = (h1_high << 4) | h1_low;
    
    // Leer H2: 0xE1 y bits [7:4] de 0xE2
    i2c_read_register(0xE1, &h2_high);
    h2_low = (buf_E2 >> 4) & 0x0F;
    par_h2 = (int16_t)((h2_high << 4) | h2_low);
    
    // Leer H3 a H7
    i2c_read_register(0xE4, (uint8_t *)&par_h3);
    i2c_read_register(0xE5, (uint8_t *)&par_h4);
    i2c_read_register(0xE6, (uint8_t *)&par_h5);
    i2c_read_register(0xE7, &buf_E7);
    par_h6 = buf_E7;
    i2c_read_register(0xE8, (uint8_t *)&par_h7);
    
    ESP_LOGI(TAG, "Calibración Humedad: H1=%u, H2=%d, H3=%d, H4=%d, H5=%d, H6=%u, H7=%d",
             par_h1, par_h2, par_h3, par_h4, par_h5, par_h6, par_h7);
}

float compensate_humidity(uint16_t hum_adc, float temp_comp)
{
    double var1, var2, var3, var4;
    var1 = hum_adc - (((double)par_h1 * 16.0) + (((double)par_h3 / 2.0) * temp_comp));
    var2 = var1 * (((double)par_h2 / 262144.0) *
            (1.0 + (((double)par_h4 / 16384.0) * temp_comp) +
             (((double)par_h5 / 1048576.0) * temp_comp * temp_comp)));
    var3 = (double)par_h6 / 16384.0;
    var4 = (double)par_h7 / 2097152.0;
    float hum_comp = var2 + ((var3 + (var4 * temp_comp)) * var2 * var2);
    return hum_comp;
}

// ============================
// Función principal
// ============================
#ifdef __cplusplus
extern "C" {
#endif

void app_main(void)
{
    esp_err_t err;
    uint16_t hum_raw;
    uint32_t raw_temp, raw_press;
    float hum_final, temp_comp, press_final;
    int32_t t_fine;

    // Inicializar I2C
    err = set_i2c();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error inicializando I2C");
        return;
    }

    // Verificar ID del chip
    uint8_t chip_id;
    i2c_read_register(0xD0, &chip_id);
    if (chip_id != 0x61) {
        ESP_LOGE(TAG, "Chip ID inválido: 0x%02X", chip_id);
        return;
    }
    ESP_LOGI(TAG, "Chip ID correcto: 0x%02X", chip_id);

    // Leer parámetros de calibración
    read_h_calibration_data();
    read_t_calibration_data();
    read_p_calibration_data();

    // Configurar modo forzado
    err = bme680_configForcedMode(BME680_OSR_1X, BME680_OSR_2X, BME680_OSR_16X);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error configurando modo forced");
        return;
    }

    // Esperar hasta que la medición esté lista
    uint8_t status;
    do {
        i2c_read_register(0x1D, &status);
        vTaskDelay(pdMS_TO_TICKS(10));
    } while (status & 0x80);

    // Leer y compensar temperatura
    raw_temp = read_raw_temperature();
    if (raw_temp == 0) {
        ESP_LOGE(TAG, "Error leyendo temperatura raw");
        return;
    }
    temp_comp = compensate_temperature(raw_temp, &t_fine);
    ESP_LOGI(TAG, "Temperatura compensada: %.2f °C", temp_comp);

    // Leer y compensar humedad
    err = read_humidity_raw(&hum_raw);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error leyendo humedad raw");
        return;
    }
    hum_final = compensate_humidity(hum_raw, temp_comp);
    //if (hum_final < 0) hum_final = 0;
    //if (hum_final > 100) hum_final = 100;
    ESP_LOGI(TAG, "Humedad compensada: %.2f %% r.H.", hum_final);

    // Leer y compensar presión
    raw_press = read_raw_pressure();
    if (raw_press == 0) {
        ESP_LOGE(TAG, "Error leyendo presión raw");
        return;
    }
    press_final = compensate_pressure(raw_press, t_fine);
    ESP_LOGI(TAG, "Presión compensada: %.2f hPa", press_final);
}

#ifdef __cplusplus
}
#endif
