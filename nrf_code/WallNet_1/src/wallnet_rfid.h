#ifndef WALLNET_RFID_H
#define WALLNET_RFID_H

#include <zephyr/kernel.h>
#include <stdint.h>
#include <stddef.h>

int wallnet_rfid_init(void);
int wallnet_rfid_write_url(const char *url);
void i2c_scan(void);
int st25dv_write_bytes(uint16_t mem_addr, uint8_t *data, size_t len);

#endif 
