#ifndef BQ_H
#define BQ_H

#include <stdint.h>

int BQ_init(void);
int BQ_status(void);
int BQ_ret_batt(void);
int BQ_write(uint8_t reg, uint8_t value);

#endif