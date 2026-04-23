#include "wallnet_rfid.h"
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <string.h>
#include <zephyr/logging/log.h>


#define ST25DV_ADDR 0x53

LOG_MODULE_REGISTER(wallnet_rfid, LOG_LEVEL_INF);

// idk the correct i2c node
#define I2C_NODE DT_NODELABEL(i2c1)

static const struct device *i2c_dev;

int wallnet_rfid_init(void)
{
    i2c_dev = DEVICE_DT_GET(I2C_NODE);

    // if (!device_is_ready(i2c_dev)) {
    //     return -ENODEV;
    // }

    printk("RFID: checking I2C device...\n");
    if (!device_is_ready(i2c_dev)) {
        LOG_WRN("I2C NOT READY\n");
        return -ENODEV;
    }

    LOG_WRN("RFID i2c Initialized");

    return 0;
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
    static uint8_t ndef[300];  // give yourself headroom

    size_t url_len = strlen(url);
    if (url_len > 220) return -EINVAL;

    int i = 0;

    // --- reserve TLV ---
    ndef[i++] = 0x03;
    int len_index = i++;  // placeholder

    // --- NDEF RECORD ---
    ndef[i++] = 0xC1; // long record
    ndef[i++] = 0x01;

    uint32_t payload_len = url_len + 1;

    ndef[i++] = (payload_len >> 24) & 0xFF;
    ndef[i++] = (payload_len >> 16) & 0xFF;
    ndef[i++] = (payload_len >> 8) & 0xFF;
    ndef[i++] = payload_len & 0xFF;

    ndef[i++] = 0x55;
    ndef[i++] = 0x04;  // URL prefix: https://

    memcpy(&ndef[i], url, url_len);
    i += url_len;

    int ndef_len = i - 2;

    // --- FIX TLV LENGTH ---
    if (ndef_len < 0xFF) {
        ndef[len_index] = ndef_len;
    } else {
        // shift data forward for extended TLV
        memmove(&ndef[len_index + 2], &ndef[len_index + 1], ndef_len);
        ndef[len_index] = 0xFF;
        ndef[len_index + 1] = (ndef_len >> 8) & 0xFF;
        ndef[len_index + 2] = ndef_len & 0xFF;
        i += 2;
    }

    ndef[i++] = 0xFE;

    return st25dv_write_bytes(0x0004, ndef, i);
}