    #ifndef __I2C_H_
    #define __I2C_H_

    #include <stdint.h>
    #include <zephyr/drivers/i2c.h>

    #define SCL_PIN 0
    #define SDA_PIN 0

    void I2C_init(void);
    int I2C_write(uint8_t slave_addr, uint8_t *p_data, uint32_t length);
    int I2C_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, uint32_t length);

    #endif