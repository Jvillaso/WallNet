#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <string.h>
#include <zephyr/logging/log.h>

#include "wallnet_rfid.h"

LOG_MODULE_REGISTER(wallnet_rfid, LOG_LEVEL_INF);


#define ST25DV_ADDR 0x53

// idk the correct i2c node
#define I2C_NODE DT_NODELABEL(i2c0)

static const struct device *i2c_dev;

int wallnet_rfid_init(void)
{
    i2c_dev = DEVICE_DT_GET(I2C_NODE);

    if (!device_is_ready(i2c_dev)) {
        return -ENODEV;
    }

    printk("RFID: checking I2C device...\n");
    if (!device_is_ready(i2c_dev)) {
        printk("I2C NOT READY\n");
        return -ENODEV;
    }

    return 0;
}


void i2c_scan(void)
{
    for (int addr = 0x4F; addr < 0x60; addr++) {

        if (i2c_write(i2c_dev, NULL, 0, addr) == 0) {
            printk("FOUND DEVICE @ 0x%02X\n", addr);
        }
        else {
            printk("no device @ 0x%02X\n", addr);
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

    k_msleep(15); // eeprom write delay

    return ret;
}

int wallnet_rfid_write_url(const char *url)
{
    uint8_t ndef[256];
    const char *payload = url;
    uint8_t uri_prefix_code = 0x00;
    size_t payload_len;

    if (strncmp(url, "https://", strlen("https://")) == 0) {
        uri_prefix_code = 0x04;
        payload = url + strlen("https://");
    } else if (strncmp(url, "http://", strlen("http://")) == 0) {
        uri_prefix_code = 0x03;
        payload = url + strlen("http://");
    }

    payload_len = strlen(payload);

    // TLV + short-record NDEF must fit in our fixed buffer.
    if (payload_len > 249) {
        return -EINVAL;
    }

    int i = 0;

    // tlv but i kinda dont know what this means
    ndef[i++] = 0x03;
    ndef[i++] = payload_len + 5; // len of NDEF record

    // NDEF record
    ndef[i++] = 0xD1; // header
    ndef[i++] = 0x01; // type len
    ndef[i++] = payload_len + 1; // payload len

    ndef[i++] = 0x55; // 'U'
    ndef[i++] = uri_prefix_code;

    memcpy(&ndef[i], payload, payload_len);
    i += payload_len;

    ndef[i++] = 0xFE; // terminator

    // IM NOT SURE IF ITS 0004 OR SOMETHING ELSE
    return st25dv_write_bytes(0x0004, ndef, i);
}
