#include "EPD_nrf.h"


/** TODO List:
 *  Set pin numbers
 *  does GPIO need to be here?
 *  Header file
 *  SPI functionality needs to get working
 *  in reset -> SPI initializes
 *  in deep sleep -> SPI disables, and set mosi and sck pins low!
 *  Picture data, I want to make a photo to display!
 */

void EP_SendCommand(uint8_t command){ //We send a command to the EINK display
    gpio_set_low(DC_PIN);
    gpio_set_low(CS_PIN);
    spi_write(&command, 1);
    gpio_set_high(CS_PIN);
}

void EP_SendData(uint8_t data){
    gpio_set_high(DC_PIN);
    gpio_set_low(CS_PIN);
    spi_write(&data, 1);
    gpio_set_high(CS_PIN);
}

int EP_Init(void) { //Initialize All registers and pins for Eink Use
    gpio_init_output(RST_PIN);  //out
    gpio_init_output(DC_PIN);   //out
    gpio_init_output(CS_PIN);   //out
    gpio_init_input(BUSY_PIN);  //in

    EP_Reset();

    EP_SendCommand(0x00);//PSR 
    EP_SendData(0xCF);//res[1:0], reg, kw/r, UD, SHL, SHD_N, RST_N
    EP_SendData(0x01);//x,x,x, vcmz, ts_auto, tige, norg, vc_lutz

    EP_SendCommand(0x01);
    EP_SendData(0x03);
    EP_SendData(0x10);
    EP_SendData(0x3F);
    EP_SendData(0x3F);

    EP_SendCommand(0x06); //may need to revisit
    EP_SendCommand(0x60);
    EP_SendData(0x22);

    EP_SendCommand(0x82);
    EP_SendData(0x07);

    EP_SendCommand(0x30);
    EP_SendData(0x09);

    EP_SendCommand(0xe3);
    EP_SendData(0x88);

    EP_SendCommand(0x61); //resolution
    EP_SendData(0xf0); //240
    EP_SendData(0x01); //360
    EP_SendData(0x68); //360

    EP_SendCommand(0x50); //Vcom and Data Interval Settings
    EP_SendData(0xB7);

    return 0;
}

void EP_Reset(void){ //Resets all Eink Registers
    //TODO: add function to initialize SPI here!!
    spi_init();
    gpio_set_high(RST_PIN);
    nrf_delay_ms(20);
    gpio_set_low(RST_PIN);
    nrf_delay_ms(2);
    gpio_set_high(RST_PIN);
    nrf_delay_ms(20); //TODO: could replace this line with 
}

void EP_ReadBusy(void){
    bool busy; //char uses less space
    do {
        busy = gpio_read(BUSY_PIN);
    }while(busy);
    nrf_delay_ms(100);
}

void EP_Refresh(void) {
    EP_SendCommand(0x17);
    EP_SendData(0xA5);
    EP_ReadBusy();
    nrf_delay_ms(100);
}

void EP_ImageBuffer(uint8_t* picdata){ //TODO: Rewrite picdata, where does it come from?
    EP_SendCommand(0x13)//write to buffer
    for(int i = 0; i < (EP_WIDTH * EP_HEIGHT/8); i++){
        EP_SendData(&picData[i]);
    }
}

void EP_Clear(bool color){
    uint8_t base = color ? 0xFF : 0x00;
    EP_SendCommand(0x13);
    for(int i = 0; i < (EP_WIDTH * EP_HEIGHT/8); i++){
        EP_SendData(base);
    }
    EP_Refresh();
}

void EP_Sleep(void){ //can only wake up on a reset!
    spi_disable();
    EP_SendCommand(0x07);
    SendData(0xA5);
}