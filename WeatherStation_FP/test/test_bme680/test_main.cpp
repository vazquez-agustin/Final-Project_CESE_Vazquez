/************************************************************************************************
 *
 *  @author     Agustín Jesús Vazquez <vazqueza193@gmail.com>
 *  @date       Mayo, 2025
 *  @version    1.0
 *
 *  @license    MIT License
 *
Copyright (c) 2025, Agustín Jesús Vazquez <vazqueza193@gmail.com>


Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*************************************************************************************************/

/** @file  BME680.cpp
 ** @brief Implementación de la clase BME680
 **/

/* === Headers files inclusions ================================================================ */
#include "MockI2C.h"
#include "DataLoggerMock.cpp"
#include "DelayManagerMock.cpp"
#include "BME680.h"
#include "BME680_defines.h"
#include <unity.h>
#include <cstdio>
/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

/**
 * @brief Fragmenta un valor crudo de 20 bits (presión o temperatura) en tres bytes, tal como los almacena el BME680 en sus registros MSB, LSB y XLSB.
 * @param raw Valor crudo de 20 bits a separar.
 * @param msb Salida: byte más significativo (bits 19–12).
 * @param lsb Salida: byte intermedio (bits 11–4).
 * @param xlsb Salida: byte menos significativo (bits 3–0), empaquetado en posiciones 7–4.
 */
static void splitRaw20(uint32_t raw, uint8_t &msb, uint8_t &lsb, uint8_t &xlsb) {
    msb  = static_cast<uint8_t>((raw >> 12) & 0xFF);
    lsb  = static_cast<uint8_t>((raw >>  4) & 0xFF);
    xlsb = static_cast<uint8_t>((raw & 0x0F) << 4);
}

/**
 * @brief Divide un valor crudo de 16 bits (humedad) en dos bytes, tal como los almacena el BME680 en sus registros MSB y LSB.
 *  @param raw  Valor crudo de 16 bits a dividir.
 *  @param msb  Salida: byte más significativo (bits 15–8).
 *  @param lsb  Salida: byte menos significativo (bits 7–0).
 */
static void splitRaw16(uint16_t raw, uint8_t &msb, uint8_t &lsb) {
    msb = static_cast<uint8_t>((raw >> 8) & 0xFF);
    lsb = static_cast<uint8_t>( raw & 0xFF);
}

I2CMock i2cMock;
DataLoggerMock dataLogger;
DelayManagerMock delayManager;

BME680 bme680 = BME680(&i2cMock, &dataLogger, &delayManager);

/**
 * @brief Inicialización previa a cada test.
 *
 * Escribe datos de calibración simulados en el mock I2C:
 * - Temperatura: par_t1=12345, par_t2=0, par_t3=0
 * - Humedad: todos los parámetros Hx = 0
 * - Presión: todos los parámetros Px = 0
 */
void setUp(void) {
    uint8_t zero = 0;
    // 1) Temperatura: Ponemos T1=12345 (cualquiera), T2=0, T3=0
    uint16_t t1 = 12345;
    uint8_t t1_lsb = t1 & 0xFF;
    uint8_t t1_msb = (t1 >> 8) & 0xFF;
    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_T1_LSB, &t1_lsb, 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_T1_MSB, &t1_msb, 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_T2_LSB, &zero, 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_T2_MSB, &zero, 1);
    i2cMock.writeRegister(BME680_I2C_ADDR, BME680_REG_T3, &zero, 1);

    // 2) Humedad: Todos los parámetros Hx = 0
    const uint8_t hum_regs[] = {
        BME680_REG_H1_LSB, BME680_REG_H1_MSB,
        BME680_REG_H2_LSB, BME680_REG_H2_MSB,
        BME680_REG_H3,
        BME680_REG_H4,
        BME680_REG_H5,
        BME680_REG_H6,
        BME680_REG_H7
    };
    for (auto r : hum_regs) {
        i2cMock.writeRegister(BME680_I2C_ADDR, r, &zero, 1);
    }

    // 3) Presión: Todos los parámetros Px = 0
    const uint8_t pres_regs[] = {
        BME680_REG_P1_LSB, BME680_REG_P1_MSB,
        BME680_REG_P2_LSB, BME680_REG_P2_MSB,
        BME680_REG_P3,
        BME680_REG_P4_LSB, BME680_REG_P4_MSB,
        BME680_REG_P5_LSB, BME680_REG_P5_MSB,
        BME680_REG_P6,
        BME680_REG_P7,
        BME680_REG_P8_LSB, BME680_REG_P8_MSB,
        BME680_REG_P9_LSB, BME680_REG_P9_MSB,
        BME680_REG_P10
    };
    for (auto r : pres_regs) {
        i2cMock.writeRegister(BME680_I2C_ADDR, r, &zero, 1);
    }
    
}

/**
 * @brief Limpieza posterior a cada test.
 */
void tearDown(void) {
    // clean stuff up here
}

/**
 * @brief Verifica que configForcedMode programe bien CTRL_HUM y CTRL_MEAS.
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

/**
 * @brief Verifica que calibration() devuelve ESP_OK.
 */
void test_calibration_returns_ok(void) {
    esp_err_t err = bme680.calibration();
    TEST_ASSERT_EQUAL_INT_MESSAGE(ESP_OK, err,
        "calibration() debió devolver ESP_OK");
}

/**
 * @brief Test de lectura cruda de temperatura: comprueba reconstrucción de 20 bits.
 */
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

/**
 * @brief Test de lectura cruda de presión: comprueba reconstrucción de 20 bits.
 */
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

/**
 * @brief Test de lectura cruda de humedad: comprueba reconstrucción de 16 bits.
 */
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

/**
 * @brief Cuando par_p1 == 0, compensatePressure debe devolver -1.0 para evitar división por cero.
 */
void test_pressure_compensation_errors_check() {

    uint32_t raw_press = 500000;  // cualquier valor de presión raw
    int32_t  t_fine    = 0;       // t_fine sin relevancia cuando par_p1==0

    float result = bme680.compensatePressure(raw_press, t_fine);

    TEST_ASSERT_EQUAL_FLOAT(-1, result);

}

/**
 * @brief Con parámetros de calibración nulos salvo par_t1, espera temp=0.0, hum=0.0, pres=-1.0.
 */
void test_ranges(void) {

    // Inicializamos calibration (lee todos los par_* y los deja =0 salvo par_t1)
    bme680.calibration();

    // Temperatura compensada -> 0.0
    int32_t dummy_tfine = 0;
    float temp = bme680.compensateTemperature(999999, &dummy_tfine);
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0f, temp,
        "Temperatura debe ser exactamente 0.0");

    // Humedad compensada -> 0.0
    float hum = bme680.compensateHumidity(12345, temp);
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0f, hum,
        "Humedad debe ser exactamente 0.0");

    // Presión compensada -> -1.0 (error por par_p1==0)
    float pres = bme680.compensatePressure(54321, dummy_tfine);
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(-1.0f, pres,
        "Presión debe ser exactamente -1.0");

}

/**
 * @brief Función principal que ejecuta todos los tests.
 */
int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_calibration_returns_ok);
    RUN_TEST(test_readRawTemperature);
    RUN_TEST(test_readRawPressure);
    RUN_TEST(test_readRawHumidity);
    RUN_TEST(test_configForcedMode_registers);
    RUN_TEST(test_pressure_compensation_errors_check);
    RUN_TEST(test_ranges);
    return UNITY_END();
}
