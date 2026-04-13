#include "spi_nrf.h"

void spi_init(void) {
    NRF_SPIM0->PSEL.SCK  = SCK_PIN; 
    NRF_SPIM0->PSEL.MOSI = MOSI_PIN;

    NRF_SPIM0->PSEL.MISO = 0xFFFFFFFF; //Disconnect

    // Frequency and Config as usual
    NRF_SPIM0->FREQUENCY = 0x40000000;
    NRF_SPIM0->CONFIG = 0;
    NRF_SPIM0->ENABLE = 7;
}
void spi_disable(void) {
    NRF_SPIM0->ENABLE = 0;
    NRF_P0->OUTCLR = (1 << MOSI_PIN) | (1 << SCK_PIN);
}
void spi_write(uint8_t *data, int len){
    NRF_SPIM0->TXD.PTR = (uint32_t)data; // Pointer to the data
    NRF_SPIM0->TXD.MAXCNT = len;
    
    NRF_SPIM0->TASKS_START = 1;
    while (NRF_SPIM0->EVENTS_END == 0);
    NRF_SPIM0->EVENTS_END = 0;
}
