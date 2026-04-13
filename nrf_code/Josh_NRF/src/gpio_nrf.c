#include "gpio_nrf.h"

void gpio_init_output(uint32_t pin_num){
    NRF_P0->DIRSET = (1 << pin_num);
}
void gpio_init_input(uint32_t pin_num){
    NRF_P0->DIRCLR = (1 << pin_num);
}
void gpio_set_high(uint32_t pin_num) {
    NRF_P0->OUTSET = (1 << pin_num);
}
void gpio_set_low(uint32_t pin_num) {
    NRF_P0->OUTCLR = (1 << pin_num);
}
bool gpio_read(uint32_t pin_num) {
    return (NRF_P0->IN >> pin_num) & 1;
}