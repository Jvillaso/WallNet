#include "i2c_nrf.h"

void I2C_init(void){
    NRF_TWIM1->PSEL.SCL = SCL_PIN;
    NRF_TWIM1->PSEL.SDA = SDA_PIN;

    NRF_TWIM1->FREQUENCY = TWIM_FREQUENCY_FREQUENCY_K400;
    NRF_TWIM1->ENABLE = (TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos);
}

int I2C_write(uint8_t slave_addr, uint8_t *p_data, uint32_t length) {
    NRF_TWIM1->ADDRESS = slave_addr;

    NRF_TWIM1->TXD.PTR = (uint32_t)p_data;
    NRF_TWIM1->TXD.MAXCNT = length;

    NRF_TWIM1->EVENTS_LASTTX = 0;
    NRF_TWIM1->EVENTS_ERROR  = 0;
    NRF_TWIM1->TASKS_STARTTX = 1;

    while (NRF_TWIM1->EVENTS_LASTTX == 0 && NRF_TWIM1->EVENTS_ERROR == 0); //wait for completetion

    NRF_TWIM1->TASKS_STOP = 1;
    while (NRF_TWIM1->EVENTS_STOPPED == 0);

    return (NRF_TWIM1->EVENTS_ERROR == 0) ? 0 : -1;
}

int I2C_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, uint32_t length) {
    NRF_TWIM1->ADDRESS = slave_addr;

    NRF_TWIM1->TXD.PTR = (uint32_t)&reg_addr;
    NRF_TWIM1->TXD.MAXCNT = 1;

    NRF_TWIM1->RXD.PTR = (uint32_t)data;
    NRF_TWIM1->RXD.MAXCNT = length;

    NRF_TWIM1->SHORTS = TWIM_SHORTS_LASTTX_STARTRX_Msk;

    NRF_TWIM1->EVENTS_LASTRX = 0;
    NRF_TWIM1->EVENTS_ERROR  = 0;
    NRF_TWIM1->TASKS_STARTTX = 1;

    while (NRF_TWIM1W->EVENTS_LASTRX == 0 && NRF_TWIM1->EVENTS_ERROR == 0);

    NRF_TWIM1->TASKS_STOP = 1;
    while (NRF_TWIM1->EVENTS_STOPPED == 0);
    
    NRF_TWIM1->SHORTS = 0;

    return (NRF_TWIM1->EVENTS_ERROR == 0) ? 0 : -1;
}