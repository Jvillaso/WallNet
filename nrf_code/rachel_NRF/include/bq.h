#ifndef BQ25895_H
#define BQ25895_H

#include <stdint.h>

#define BQ_ADDR 0x6A

void BQ_init(void);
int BQ_status(void);
int BQ_write(uint8_t reg, uint8_t value);
int BQ_read(uint8_t reg, uint8_t *data);
int BQ_ret_batt(void);

#endif