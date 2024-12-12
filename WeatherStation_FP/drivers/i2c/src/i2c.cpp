#include "driver/i2c.h"

// private definitions
#define I2C_MASTER_SDA_IO 8
#define I2C_MASTER_SCL_IO 9
#define I2C_MASTER_NUM I2C_NUM_0     // first i2c controller available in ESP32
#define I2C_MASTER_FREQ_HZ 100000

// Private function prototypes
static void i2c_master_init(void);
static void i2c_set_register(uint8_t reg, uint8_t value);
static uint8_t i2c_get_register(uint8_t reg);

// Private function implementations

void i2c_init(void) {
    i2c_master_init();
}


void i2c_master_init () {
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ; // define frequency
    
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}