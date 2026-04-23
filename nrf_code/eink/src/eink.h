
#ifndef EINK_H
#define EINK_H

#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <stdint.h>
#include <stdlib.h>


typedef struct {
    int valid;
    char first_name[20];
    char last_name[20];
    char last_four[6];
} card_record_t;


// Display resolution
#define EPD_WIDTH       122
#define EPD_HEIGHT      250
#define IMG_BUF_SIZE ((EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1)) * EPD_HEIGHT


#define FULL_MODE			0
#define FAST_MODE			1
#define PART_MODE			2

#define SPIOP      SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER 
#define LOW       0
#define HIGH      1

// static const struct gpio_dt_spec dc_pin;
// static const struct gpio_dt_spec busy_pin;
// static const struct gpio_dt_spec reset_pin;
// extern int dc_pin;
// extern int busy_pin;
// extern int reset_pin;

// extern int count;
// extern int bufwidth;
// extern int bufheight;



int eink_init(void);
void SpiTransfer(unsigned char data);
void DigitalWrite(int pin, int value);
int DigitalRead(int pin);


void SendCommand(unsigned char command);
void SendData(unsigned char data);
void WaitUntilIdle(void);
void SetWindows(unsigned char Xstart, unsigned char Ystart, unsigned char Xend, unsigned char Yend);
void SetCursor(unsigned char Xstart, unsigned char Ystart);
int Init(char Mode);
void Reset(void);
void Clear(void);
void Display(unsigned char* frame_buffer);
void DisplayPart(unsigned char* frame_buffer);
void Display1(unsigned char* frame_buffer);
void Display_Fast(unsigned char* frame_buffer);   
void DisplayPartBaseImage(unsigned char* frame_buffer);
void DisplayPart(unsigned char* frame_buffer);
void ClearPart(void);
void DisplayPartial(int x_start, int y_start, int x_end, int y_end, unsigned char* frame_buffer);
void Sleep();

#endif // EINK_H
