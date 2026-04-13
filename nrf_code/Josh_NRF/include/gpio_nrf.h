/**
 * 
 */

 #ifndef __GPIO_H_
 #define __GPIO_H_

 #include <stdint.h>
 #include "nrf.h"

 void gpio_init_output(uint32_t pin_num);
 void gpio_init_input(uint32_t pin_num);
 void gpio_set_high(uint32_t pin_num);
 void gpio_set_low(uint32_t pin_num);
 bool gpio_read(uint32_t pin_num);

 #endif