#include "MockI2C.h"
#include "BME680.h"
#include <unity.h>

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

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

I2CMock i2cMock;
BME680 bme680 = BME680(&i2cMock);

void test_readRawTemperature(void) {
    
    // Simula el chip ID correcto para evitar errores de inicialización
    uint8_t chip_id_val = BME680_CHIP_ID;
    i2cMock.writeRegister(BME680_I2C_ADDR, REG_CHIP_ID, &chip_id_val, 1);

    uint32_t expectedTempRaw = 0xABCDE;  // Valor raw de temperatura conocido a simular (20 bits)
    uint8_t temp_msb = (expectedTempRaw >> 12) & 0xFF;              // byte MSB (bits 19-12)
    uint8_t temp_lsb = (expectedTempRaw >> 4) & 0xFF;               // byte LSB (bits 11-4)
    uint8_t temp_xlsb = (uint8_t)((expectedTempRaw & 0xF) << 4);    // byte XLSB con los 4 bits bajos (bits 3-0) en las posiciones 7-4

    i2cMock.writeRegister(BME680_I2C_ADDR, REG_TEMP_MSB, &temp_msb, 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, REG_TEMP_LSB, &temp_lsb, 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, REG_TEMP_XLSB, &temp_xlsb, 1);

    uint32_t actualTempRaw = bme680.readRawTemperature();

    TEST_ASSERT_EQUAL_HEX32(expectedTempRaw, actualTempRaw);
}

void test_readRawPressure(void) {

    uint8_t chip_id_val = BME680_CHIP_ID;
    i2cMock.writeRegister(BME680_I2C_ADDR, REG_CHIP_ID, &chip_id_val, 1);

    uint32_t expectedPressRaw = 0x54321;  // Valor raw de presión conocido a simular (20 bits)
    uint8_t press_msb = (expectedPressRaw >> 12) & 0xFF;            // byte MSB (bits 19-12)
    uint8_t press_lsb = (expectedPressRaw >> 4) & 0xFF;             // byte LSB (bits 11-4)
    uint8_t press_xlsb = (uint8_t)((expectedPressRaw & 0xF) << 4);  // byte XLSB con bits 3-0 en posiciones 7-4

    i2cMock.writeRegister(BME680_I2C_ADDR, REG_PRESS_MSB, &press_msb, 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, REG_PRESS_LSB, &press_lsb, 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, REG_PRESS_XLSB, &press_xlsb, 1);

    uint32_t actualPressRaw = bme680.readRawPressure();

    TEST_ASSERT_EQUAL_HEX32(expectedPressRaw, actualPressRaw);
}

void test_readRawHumidity(void) {

    uint8_t chip_id_val = BME680_CHIP_ID;
    i2cMock.writeRegister(BME680_I2C_ADDR, REG_CHIP_ID, &chip_id_val, 1);

    uint16_t expectedHumRaw = 0x7BDE;  // Valor raw de humedad conocido a simular (16 bits)
    uint8_t hum_msb = (expectedHumRaw >> 8) & 0xFF;   // byte MSB (bits 15-8)
    uint8_t hum_lsb = expectedHumRaw & 0xFF;         // byte LSB (bits 7-0)

    i2cMock.writeRegister(BME680_I2C_ADDR, REG_HUM_MSB, &hum_msb, 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, REG_HUM_LSB, &hum_lsb, 1);

    uint32_t actualHumRaw = bme680.readRawHumidity();  // Nota: la función podría devolver uint16; usamos 32 bits para comparar

    TEST_ASSERT_EQUAL_HEX32(expectedHumRaw, actualHumRaw);
}

// Cuando no hay parámetros de calibración (todos en 0),
// devolvemos 0 para evitar divisiones por cero o resultados inválidos.

void test_temperature_compensation_defaults_to_zero() {

    int32_t t_fine = 123456;
    uint32_t raw_temp = 500000;

    float result = bme680.compensateTemperature(raw_temp, &t_fine);

    // Con par_t1=par_t2=par_t3=0, var1=var2=0 -> t_fine=0 -> temp=0
    TEST_ASSERT_EQUAL_INT(0, t_fine);
    TEST_ASSERT_EQUAL_FLOAT(0, result);

}

void test_pressure_compensation_defaults_to_zero() {

    uint32_t raw_press = 500000;  // cualquier valor de presión raw
    int32_t  t_fine    = 0;       // t_fine sin relevancia cuando par_p1==0

    float result = bme680.compensatePressure(raw_press, t_fine);

    TEST_ASSERT_EQUAL_FLOAT(0.0f, result);

}

void test_humidity_compensation_defaults_to_zero(void) {
    
    float temp_comp = 25.0f;
    uint16_t raw_hum = 30000;

    float result = bme680.compensateHumidity(raw_hum, temp_comp);

    TEST_ASSERT_EQUAL_FLOAT(0.0f, result);
}

void test_ranges(void) {
    int32_t t_fine = 0;

    // 1) Temperatura: raw cercano a 20°C
    uint32_t raw_temp = 519888;  
    float temp = bme680.compensateTemperature(raw_temp, &t_fine);
    // Rango logico en Irlanda: -5°C a 30°C
    TEST_ASSERT_TRUE_MESSAGE(temp >= -5.0f && temp <= 30.0f,
        "Temperatura fuera de rango [-5,30]");

    // 2) Humedad: raw cualquiera medio
    uint16_t raw_hum = 30000;
    float hum = bme680.compensateHumidity(raw_hum, temp);
    // La humedad debe estar entre 0% y 100%
    TEST_ASSERT_TRUE_MESSAGE(hum >= 0.0f && hum <= 100.0f,
        "Humedad fuera de rango [0,100]");

    // 3) Presión: usando el t_fine calculado arriba
    uint32_t raw_press = 500000;
    float press = bme680.compensatePressure(raw_press, t_fine);
    // En Irlanda ~950–1200 hPa
    TEST_ASSERT_TRUE_MESSAGE(press >= 950.0f && press <= 1200.0f,
        "Presion fuera de rango [950,1200]");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_readRawTemperature);
    RUN_TEST(test_readRawPressure);
    RUN_TEST(test_readRawHumidity);
    RUN_TEST(test_temperature_compensation_defaults_to_zero);
    RUN_TEST(test_pressure_compensation_defaults_to_zero);
    RUN_TEST(test_humidity_compensation_defaults_to_zero);
    RUN_TEST(test_ranges);
    return UNITY_END();
}
