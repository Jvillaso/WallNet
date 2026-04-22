#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <string.h>
#include <zephyr/logging/log.h>

#include "wallnet_rfid.h"

LOG_MODULE_REGISTER(wallnet_rfid, LOG_LEVEL_INF);


#define ST25DV_ADDR 0x53

// idk the correct i2c node
#define I2C_NODE DT_NODELABEL(i2c1)

static const struct device *i2c_dev;

void wallnet_rfid_init(void)
{
    i2c_dev = DEVICE_DT_GET(I2C_NODE);

    if (!device_is_ready(i2c_dev)) {
        LOG_ERR("RFID INIT FAILED: i2c DEVICE NOT READY\n");
        return;
    }

    LOG_WRN("RFID INITIALIZED.");

}


void i2c_scan(void)
{
    for (int addr = 0x08; addr < 0x78; addr++) {

        if (i2c_write(i2c_dev, NULL, 0, addr) == 0) {
            printk("FOUND DEVICE @ 0x%02X\n", addr);
        }
        else {
            //printk("no device @ 0x%02X\n", addr);
        }
    }
}


int st25dv_write_bytes(uint16_t mem_addr, uint8_t *data, size_t len)
{
    uint8_t buf[258];

    buf[0] = (mem_addr >> 8) & 0xFF;
    buf[1] = mem_addr & 0xFF;

    memcpy(&buf[2], data, len);

    int ret = i2c_write(i2c_dev, buf, len + 2, ST25DV_ADDR);

    k_msleep(5); // eeprom write delay

    return ret;
}

int wallnet_rfid_write_url(const char *url)
{
    uint8_t ndef[256];
    size_t url_len = strlen(url);

    // check valid
    if (url_len > 200) return -EINVAL;

    int i = 0;

    // tlv but i kinda dont know what this means
    ndef[i++] = 0x03;
    ndef[i++] = url_len + 5; // len of NDEF record

    // NDEF record
    ndef[i++] = 0xD1; // header
    ndef[i++] = 0x01; // type len
    ndef[i++] = url_len + 1; // payload len

    ndef[i++] = 0x55; // 'U'
    ndef[i++] = 0x04; // https:// prefix

    memcpy(&ndef[i], url, url_len);
    i += url_len;

    ndef[i++] = 0xFE; // terminator

    // IM NOT SURE IF ITS 0004 OR SOMETHING ELSE
    return st25dv_write_bytes(0x0004, ndef, i);
}