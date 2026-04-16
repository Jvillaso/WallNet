#ifndef WALLNET_RFID_H
#define WALLNET_RFID_H

#include <zephyr/kernel.h>
#include <stdint.h>

int wallnet_rfid_init(void);
int wallnet_rfid_write_url(const char *url);

#endif