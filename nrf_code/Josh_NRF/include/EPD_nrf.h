/**
 * Header Info
 * 
*/ 
    #ifndef __EPD_H_
    #define __EPD_H_

    #include <stdint.h>
    #include "gpio_nrf.h"
    #include "spi_nrf.h"
    #include "nrf.h"


    #define EP_WIDTH 360
    #define EP_HEIGHT 240

    #define RST_PIN 0
    #define DC_PIN 0
    #define CS_PIN 0
    #define BUSY_PIN 0

    #define MOSI_PIN 0
    #define SCK_PIN 0

    void EP_SendCommand(uint8_t command);
    void EP_SendData(uint8_t data);
    void EP_Init(void);
    void EP_Reset(void);
    void EP_ReadBusy(void);
    void EP_Refresh(void);
    void EP_ImageBuffer(uint8_t* picdata);
    void EP_Clear(bool color);
    void EP_Sleep(void);

    #endif 