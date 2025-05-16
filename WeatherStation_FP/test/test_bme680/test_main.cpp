#include "MockI2C.h"
#include "BME680.h"
#include <unity.h>
#include <cstdio>

// Definiciones de registros del BME680 (direcciones I2C de los registros MSB/LSB/XLSB y chip ID)
const uint8_t BME680_I2C_ADDR   = 0x77;    // Dirección I2C del BME680
const uint8_t REG_PRESS_MSB     = 0x1F;
const uint8_t REG_PRESS_LSB     = 0x20;
const uint8_t REG_PRESS_XLSB    = 0x21;
const uint8_t REG_TEMP_MSB      = 0x22;
const uint8_t REG_TEMP_LSB      = 0x23;
const uint8_t REG_TEMP_XLSB     = 0x24;
const uint8_t REG_HUM_MSB       = 0x25;
const uint8_t REG_HUM_LSB       = 0x26;
const uint8_t REG_CHIP_ID       = 0xD0;
const uint8_t BME680_CHIP_ID    = 0x61;    // Valor esperado del chip ID del BME680
const uint8_t BME680_REG_T1_MSB = 0xE9;
const uint8_t BME680_REG_T1_LSB = 0xEA;
const uint8_t BME680_REG_T2_MSB = 0x8A;
const uint8_t BME680_REG_T2_LSB = 0x8B;
const uint8_t BME680_REG_T3     = 0x8C;

const uint8_t BME680_REG_H1_MSB = 0xE3;
const uint8_t BME680_REG_H1_LSB = 0xE2;
const uint8_t BME680_REG_H2_MSB = 0xE1;
const uint8_t BME680_REG_H2_LSB = 0xE2;
const uint8_t BME680_REG_H3     = 0xE4;
const uint8_t BME680_REG_H4     = 0xE5;
const uint8_t BME680_REG_H5     = 0xE6;
const uint8_t BME680_REG_H6     = 0xE7;
const uint8_t BME680_REG_H7     = 0xE8;

const uint8_t BME680_REG_P1_MSB = 0x8F;
const uint8_t BME680_REG_P1_LSB = 0x8E;
const uint8_t BME680_REG_P2_MSB = 0x91;
const uint8_t BME680_REG_P2_LSB = 0x90;
const uint8_t BME680_REG_P3     = 0x92;
const uint8_t BME680_REG_P4_MSB = 0x95;
const uint8_t BME680_REG_P4_LSB = 0x94;
const uint8_t BME680_REG_P5_MSB = 0x97;
const uint8_t BME680_REG_P5_LSB = 0x96;
const uint8_t BME680_REG_P6     = 0x99;
const uint8_t BME680_REG_P7     = 0x98;
const uint8_t BME680_REG_P8_MSB = 0x9D;
const uint8_t BME680_REG_P8_LSB = 0x9C;
const uint8_t BME680_REG_P9_MSB = 0x9F;
const uint8_t BME680_REG_P9_LSB = 0x9E;
const uint8_t BME680_REG_P10    = 0xA0;

const uint8_t BME680_REG_CTRL_HUM  = 0x72;
const uint8_t BME680_REG_CTRL_MEAS = 0x74;
const uint8_t BME680_MODE_FORCED   = 0x01;

const uint8_t BME680_OSR_2X = 0x02;
const uint8_t BME680_OSR_4X = 0x03;
const uint8_t BME680_OSR_8X = 0x04;

/// @brief Extrae los tres bytes de un valor crudo de 20 bits.
/// @param raw   Valor de 20 bits a separar.
/// @param msb   Byte más significativo (bits 19–12).
/// @param lsb   Byte intermedio (bits 11–4).
/// @param xlsb  Byte menos significativo (bits 3–0), ubicado en posiciones 7–4.
static void splitRaw20(uint32_t raw, uint8_t &msb, uint8_t &lsb, uint8_t &xlsb) {
    msb  = static_cast<uint8_t>((raw >> 12) & 0xFF);
    lsb  = static_cast<uint8_t>((raw >>  4) & 0xFF);
    xlsb = static_cast<uint8_t>((raw & 0x0F) << 4);
}

/// @brief Separa un valor crudo de 16 bits en dos bytes.
/// @param raw  Valor de 16 bits a dividir.
/// @param msb  Byte más significativo (bits 15–8).
/// @param lsb  Byte menos significativo (bits 7–0).
static void splitRaw16(uint16_t raw, uint8_t &msb, uint8_t &lsb) {
    msb = static_cast<uint8_t>((raw >> 8) & 0xFF);
    lsb = static_cast<uint8_t>( raw        & 0xFF);
}

I2CMock i2cMock;
BME680 bme680 = BME680(&i2cMock);

void setUp(void) {

    uint8_t calib_data_temp[5] = {1, 1, 1, 1, 1}; // Datos de calibración simulados
    
    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_T1_MSB, &calib_data_temp[0], 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_T1_LSB, &calib_data_temp[1], 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_T2_MSB, &calib_data_temp[2], 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_T2_LSB, &calib_data_temp[3], 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_T3, &calib_data_temp[4], 1);

    uint16_t par_h1 = 721;
    uint16_t par_h2 = 733;
    uint8_t  par_h3 = 0;
    uint8_t  par_h4 = 45;
    uint8_t  par_h5 = 20;
    uint8_t  par_h6 = 120;
    uint8_t  par_h7 = -100;

    // 1) H1_LSB (0xE2) guarda solo low nibble de par_h1 en bits [3:0]
    //    y también high nibble de par_h2 en bits [7:4]
    uint8_t h1_lsb = ((par_h2 & 0x0F) << 4) | (par_h1 & 0x0F);
    // 2) H1_MSB (0xE3) guarda high byte de par_h1 (bits [11:4])
    uint8_t h1_msb = (par_h1 >> 4) & 0xFF;

    // 3) Ahora escribe en el mock exactamente esos registros:
    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_H1_LSB, &h1_lsb, 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_H1_MSB, &h1_msb, 1);

    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_H3, (uint8_t[]){par_h3}, 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_H4, (uint8_t[]){par_h4}, 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_H5, (uint8_t[]){par_h5}, 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_H6, (uint8_t[]){par_h6}, 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_H7, (uint8_t[]){par_h7}, 1);

    uint16_t par_p1  = 37435;   
    int16_t  par_p2  = -10191; // 4294957105 – 2³² = -10191
    int8_t   par_p3  = 88;   
    int16_t  par_p4  = 6352;
    int16_t  par_p5  = -217; // 4294967079 – 2³² = -217
    int8_t   par_p6  = 30;
    int8_t   par_p7  = 55;
    int16_t  par_p8  = -4208; // 4294963088 – 2³² = -4208
    int16_t  par_p9  = -1957; // 4294965339 – 2³² = -1957
    uint8_t  par_p10 = 30;

    auto write16 = [&](uint8_t reg_lsb, int32_t value){
        uint8_t lsb = value & 0xFF;
        uint8_t msb = (value >> 8) & 0xFF;
        i2cMock.writeRegister(BME680_I2C_ADDR, reg_lsb,   &lsb, 1);
        i2cMock.writeRegister(BME680_I2C_ADDR, reg_lsb+1, &msb, 1);
    };

    // 1) P1 y P2:
    write16(BME680_REG_P1_LSB, par_p1);
    write16(BME680_REG_P2_LSB, par_p2);

    // 2) P3 (un solo byte int8_t):
    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_P3, (uint8_t[]){(uint8_t)par_p3}, 1);

    // 3) P4 y P5:
    write16(BME680_REG_P4_LSB, par_p4);
    write16(BME680_REG_P5_LSB, par_p5);

    // 4) P6 y P7 (cada uno un solo byte int8_t):
    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_P6, (uint8_t[]){(uint8_t)par_p6}, 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_P7, (uint8_t[]){(uint8_t)par_p7}, 1);

    // 5) P8 y P9:
    write16(BME680_REG_P8_LSB, par_p8);
    write16(BME680_REG_P9_LSB, par_p9);

    // 6) P10 (uint8_t):
    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_P10, (uint8_t[]){par_p10}, 1);
    
}

void tearDown(void) {
    // clean stuff up here
}

/**
 * @brief Verifica que configForcedMode programe bien CTRL_HUM y CTRL_MEAS
 */
void test_configForcedMode_registers(void) {
    // 1) Elige oversampling: hum=2x, temp=4x, pres=16x
    const uint8_t HUM_OSR  = BME680_OSR_2X;   // 0x02
    const uint8_t TEMP_OSR = BME680_OSR_4X;   // 0x03
    const uint8_t PRES_OSR = BME680_OSR_8X;  // 0x05

    esp_err_t err = bme680.configForcedMode(HUM_OSR, TEMP_OSR, PRES_OSR);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ESP_OK, err, "configForcedMode devolvió error");

    // 2) CTRL_HUM: solo bits [2:0] → debe contener HUM_OSR
    uint8_t ctrl_hum = i2cMock.getRegisterValue(BME680_REG_CTRL_HUM);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(HUM_OSR, ctrl_hum & 0x07,
        "CTRL_HUM no programó bien el oversampling de humedad");

    // 3) CTRL_MEAS: bits [7:5]=TEMP_OSR, [4:2]=PRES_OSR, [1:0]=modo(forced=0x01)
    uint8_t ctrl_meas = i2cMock.getRegisterValue(BME680_REG_CTRL_MEAS);
    uint8_t got_temp = (ctrl_meas >> 5) & 0x07;
    uint8_t got_pres = (ctrl_meas >> 2) & 0x07;
    uint8_t got_mode = ctrl_meas & 0x03;

    TEST_ASSERT_EQUAL_HEX8_MESSAGE(TEMP_OSR, got_temp,
        "CTRL_MEAS no programó bien el oversampling de temperatura");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(PRES_OSR, got_pres,
        "CTRL_MEAS no programó bien el oversampling de presión");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(BME680_MODE_FORCED, got_mode,
        "CTRL_MEAS no programó bien el modo forced");
}

void test_calibration_returns_ok(void) {
    esp_err_t err = bme680.calibration();
    TEST_ASSERT_EQUAL_INT_MESSAGE(ESP_OK, err,
        "calibration() debió devolver ESP_OK");
}

void test_readRawTemperature(void) {

    // Simula el chip ID correcto para evitar errores de inicialización
    uint8_t chip_id_val = BME680_CHIP_ID;
    i2cMock.writeRegister(BME680_I2C_ADDR, REG_CHIP_ID, &chip_id_val, 1);

    uint32_t expectedTempRaw = 0xABCDE;
    uint8_t temp_msb, temp_lsb, temp_xlsb;
    splitRaw20(expectedTempRaw, temp_msb, temp_lsb, temp_xlsb);

    i2cMock.writeRegister(BME680_I2C_ADDR, REG_TEMP_MSB, &temp_msb, 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, REG_TEMP_LSB, &temp_lsb, 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, REG_TEMP_XLSB, &temp_xlsb, 1);

    uint32_t actualTempRaw = bme680.readRawTemperature();
    TEST_ASSERT_EQUAL_HEX32(expectedTempRaw, actualTempRaw);
}

void test_readRawPressure(void) {

    uint8_t chip_id_val = BME680_CHIP_ID;
    i2cMock.writeRegister(BME680_I2C_ADDR, REG_CHIP_ID, &chip_id_val, 1);

    uint32_t expectedPressRaw = 0x54321;
    uint8_t press_msb, press_lsb, press_xlsb;
    splitRaw20(expectedPressRaw, press_msb, press_lsb, press_xlsb);

    i2cMock.writeRegister(BME680_I2C_ADDR, REG_PRESS_MSB, &press_msb, 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, REG_PRESS_LSB, &press_lsb, 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, REG_PRESS_XLSB, &press_xlsb, 1);

    uint32_t actualPressRaw = bme680.readRawPressure();

    TEST_ASSERT_EQUAL_HEX32(expectedPressRaw, actualPressRaw);
}

void test_readRawHumidity(void) {

    uint8_t chip_id_val = BME680_CHIP_ID;
    i2cMock.writeRegister(BME680_I2C_ADDR, REG_CHIP_ID, &chip_id_val, 1);

    uint16_t expectedHumRaw = 0x7BDE;
    uint8_t hum_msb, hum_lsb;
    splitRaw16(expectedHumRaw, hum_msb, hum_lsb);

    i2cMock.writeRegister(BME680_I2C_ADDR, REG_HUM_MSB, &hum_msb, 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, REG_HUM_LSB, &hum_lsb, 1);

    uint32_t actualHumRaw = bme680.readRawHumidity();  // Nota: la función devuelve uint16; pero uso 32 bits para comparar

    TEST_ASSERT_EQUAL_HEX32(expectedHumRaw, actualHumRaw);
}

// Cuando no hay parámetros de calibración (todos en 0),
// devolvemos 0 para evitar divisiones por cero o resultados inválidos.

void test_pressure_compensation_errors_check() {

    uint32_t raw_press = 500000;  // cualquier valor de presión raw
    int32_t  t_fine    = 0;       // t_fine sin relevancia cuando par_p1==0

    float result = bme680.compensatePressure(raw_press, t_fine);

    TEST_ASSERT_EQUAL_FLOAT(-1, result);

}

void test_ranges(void) {

    bme680.calibration();
    int32_t t_fine = 109970;

    // 1) Temperatura: raw cercano a 20°C
    uint32_t raw_temp = 483960; // valor simulado cercano a 21°C
    float temp = bme680.compensateTemperature(raw_temp, &t_fine);
 
    // Rango logico en Irlanda: 20°C a 22°C
    TEST_ASSERT_TRUE_MESSAGE(temp >= 21.0f && temp <= 23.0f,
        "Temperatura fuera de rango [20,22]");

    // 2) Humedad: raw cualquiera medio
    uint16_t raw_hum = 24612; // valor simulado cercano a 47%
    float hum = bme680.compensateHumidity(raw_hum, temp);
    // La humedad debe estar entre 0% y 100%
    TEST_ASSERT_TRUE_MESSAGE(hum >= 47.0f && hum <= 49.0f,
        "Humedad fuera de rango [47,49]");

    // 3) Presión: usando el t_fine calculado arriba
    uint32_t raw_press = 338792; // valor simulado cercano a 1004 hPa
    float press = bme680.compensatePressure(raw_press, t_fine);

    TEST_ASSERT_TRUE_MESSAGE(press >= 1002.0f && press <= 1006.0f,
        "Presion fuera de rango [1003,1005]");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_readRawTemperature);
    RUN_TEST(test_readRawPressure);
    RUN_TEST(test_readRawHumidity);
    RUN_TEST(test_pressure_compensation_errors_check);
    RUN_TEST(test_ranges);
    RUN_TEST(test_configForcedMode_registers);
    RUN_TEST(test_calibration_returns_ok);
    return UNITY_END();
}
