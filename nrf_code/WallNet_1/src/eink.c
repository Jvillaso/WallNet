#include "eink.h"
#include <zephyr/drivers/spi.h>
#include <zephyr/device.h>
#include <hal/nrf_gpio.h>
#include <zephyr/sys/printk.h>



//SPI pin
struct spi_dt_spec spispec = SPI_DT_SPEC_GET(DT_NODELABEL(gendev), SPIOP, 0); //SPI_DT_SPEC_GET(DT_NODELABEL(gendev), SPIOP, 10);


//GPIO pins
static int cs_pin = 3;
static int reset_pin = 29;
static int busy_pin = 31;
static int dc_pin = 4;
// static int test_pin = 17;

//static int debug_pin = 30;



static int count;
static int bufwidth;
static int bufheight;


//Initialize gpios
int eink_init(void) {
    // nrf_gpio_cfg(
    //     dc_pin,
    //     NRF_GPIO_PIN_DIR_OUTPUT,
    //     NRF_GPIO_PIN_INPUT_DISCONNECT,
    //     NRF_GPIO_PIN_NOPULL,
    //     NRF_GPIO_PIN_H0H1,   // ← HIGH DRIVE
    //     NRF_GPIO_PIN_NOSENSE
    // );
    nrf_gpio_cfg_output(dc_pin);
    nrf_gpio_cfg_output(cs_pin);
    nrf_gpio_cfg_input(busy_pin, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_output(reset_pin);
    //nrf_gpio_cfg_output(debug_pin);
    //nrf_gpio_cfg_output(test_pin);

    bufwidth = 128/8;  //16
    bufheight = 63;
    count = 0;

    //CS and DC pins should be HIGH by default
    DigitalWrite(cs_pin, HIGH);
    DigitalWrite(dc_pin, HIGH);

    //Reset pin initialization
    DigitalWrite(reset_pin, HIGH);
    //DigitalWrite(debug_pin, LOW);
    //DigitalWrite(test_pin, HIGH);
    Init(FULL_MODE);
    //Clear();
    return 0;
}

//Write a value to a GPIO pin
void DigitalWrite(int pin, int value) {
    if (value == LOW) {
        nrf_gpio_pin_clear(pin);
    } else {
        nrf_gpio_pin_set(pin);
    }
    
}

//Read a value from a GPIO pin
int DigitalRead(int pin) {
    return nrf_gpio_pin_read(pin);
}

//Send a byte over SPI
void SpiTransfer(unsigned char data) {
    if (!device_is_ready(spispec.bus)) {
        printk("SPI bus not ready\n");
        return;
    }
    DigitalWrite(cs_pin, LOW);
    struct spi_buf buf = {
        .buf = &data,
        .len = 1
    };
    struct spi_buf_set tx_bufs = {
        .buffers = &buf,
        .count = 1
    };
    int ret = spi_write_dt(&spispec, &tx_bufs);
    if (ret < 0) {
        // Handle error (e.g., log or return)
        printk("SPI transfer failed: %d\n", ret);
    }
    DigitalWrite(cs_pin, HIGH);
}



/******************************************************************************
function :	send command
parameter:
     command : Command register
******************************************************************************/
void SendCommand(unsigned char command)
{
    DigitalWrite(dc_pin, LOW);
    SpiTransfer(command);
    //DigitalWrite(dc_pin, HIGH);
    //k_sleep(K_MSEC(1)); // Add a small delay after sending the command
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
void SendData(unsigned char data)
{
    DigitalWrite(dc_pin, HIGH);
    SpiTransfer(data);
    //k_sleep(K_MSEC(1)); // Add a small delay after sending the data
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/
void WaitUntilIdle(void)
{
    int busy_count;
    for(busy_count = 0; busy_count < 1000; busy_count++) {      //LOW: idle, HIGH: busy
        if(DigitalRead(busy_pin) == 0) { //TODO: check this? In the provided code it was 0 but it seems like the eink is pulling it low when busy
            break;
        }
        k_sleep(K_MSEC(10));
        
    }

    //printk("Waited for %d ms\n", (busy_count) * 1);
}

/******************************************************************************
function :	Setting the display window
parameter:
	Xstart : X-axis starting position
	Ystart : Y-axis starting position
	Xend : End position of X-axis
	Yend : End position of Y-axis
******************************************************************************/
void SetWindows(unsigned char Xstart, unsigned char Ystart, unsigned char Xend, unsigned char Yend)
{
    SendCommand(0x44); // SET_RAM_X_ADDRESS_START_END_POSITION
    SendData((Xstart>>3) & 0xFF);
    SendData((Xend>>3) & 0xFF);
	
    SendCommand(0x45); // SET_RAM_Y_ADDRESS_START_END_POSITION
    SendData(Ystart & 0xFF);
    SendData((Ystart >> 8) & 0xFF);
    SendData(Yend & 0xFF);
    SendData((Yend >> 8) & 0xFF);
}

/******************************************************************************
function :	Set Cursor
parameter:
	Xstart : X-axis starting position
	Ystart : Y-axis starting position
******************************************************************************/
void SetCursor(unsigned char Xstart, unsigned char Ystart)
{
    SendCommand(0x4E); // SET_RAM_X_ADDRESS_COUNTER
    SendData(Xstart & 0xFF);

    SendCommand(0x4F); // SET_RAM_Y_ADDRESS_COUNTER
    SendData(Ystart & 0xFF);
    SendData((Ystart >> 8) & 0xFF);
}



/******************************************************************************
function :	Initialize the e-Paper register
parameter:
	Mode : Mode selection
******************************************************************************/
int Init(char Mode)
{
    /* this calls the peripheral hardware interface, see epdif */
    Reset();
    
    //int count;
    if(Mode == FULL_MODE) {
        WaitUntilIdle();
        SendCommand(0x12); // soft reset
        WaitUntilIdle();

        //DigitalWrite(debug_pin, HIGH);
        SendCommand(0x01); //Driver output control
        SendData(0xF9);
        SendData(0x00);
        SendData(0x00);

        SendCommand(0x11); //data entry mode
        SendData(0x03);

		SetWindows(0, 0, EPD_WIDTH-1, EPD_HEIGHT-1);
		SetCursor(0, 0);
		
		SendCommand(0x3C); //BorderWavefrom
		SendData(0x05);	

		SendCommand(0x21); //  Display update control
		SendData(0x00);
		SendData(0x80);	

		SendCommand(0x18); //Read built-in temperature sensor
		SendData(0x80);	
		WaitUntilIdle();
    } 
    else if(Mode == FAST_MODE) {
        WaitUntilIdle();
        SendCommand(0x12); // soft reset
        WaitUntilIdle();

        SendCommand(0x18); // Read built-in temperature sensor
		SendData(0x80);	

        SendCommand(0x11); // data entry mode
        SendData(0x03);

		SetWindows(0, 0, EPD_WIDTH-1, EPD_HEIGHT-1);
		SetCursor(0, 0);
		
		SendCommand(0x22); // Load temperature value
		SendData(0xB1);	
        SendCommand(0x20);
        WaitUntilIdle();

		SendCommand(0x1A); //  Write to temperature register
		SendData(0x64);
		SendData(0x00);	

        SendCommand(0x22); //  Load temperature value
		SendData(0x91);
		SendCommand(0x20);	
		WaitUntilIdle();
    }
    else if(Mode == PART_MODE) {	
		DigitalWrite(reset_pin, LOW);                //module reset
		k_sleep(K_MSEC(1));
		DigitalWrite(reset_pin, HIGH);
		
		SendCommand(0x3C); //BorderWavefrom
		SendData(0x80);	

        SendCommand(0x01); //Driver output control
		SendData(0xF9);	
        SendData(0x00);	
        SendData(0x00);	
	
		SendCommand(0x11); // data entry mode
        SendData(0x03); 
		
		SetWindows(0, 0, EPD_WIDTH-1, EPD_HEIGHT-1);
		SetCursor(0, 0);
    } else {
        return -1;
    }

    return 0;
}

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
void Reset(void)
{
    DigitalWrite(reset_pin, HIGH);
    k_sleep(K_MSEC(2));
    DigitalWrite(reset_pin, LOW);     
    k_sleep(K_MSEC(1));
    DigitalWrite(reset_pin, HIGH);
    k_sleep(K_MSEC(2));
    count = 0; 
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void Clear(void)
{
    Reset();
    int w, h;
    w = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    h = EPD_HEIGHT;
    SendCommand(0x24);
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            SendData(0xff);
        }
    }

    //DISPLAY REFRESH
    SendCommand(0x22);
    SendData(0xf7);
    SendCommand(0x20);
    WaitUntilIdle();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
	frame_buffer : Image data
******************************************************************************/
void Display(unsigned char* frame_buffer)
{
    int w = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    int h = EPD_HEIGHT;

    if (frame_buffer != NULL) {
        SendCommand(0x24);
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                SendData(frame_buffer[i + j * w]);
                //printk("%02x", frame_buffer[i + j * w]);
            }
            //printk("\n");
        }
    }

    
    
    
    //DISPLAY REFRESH
    SendCommand(0x22);
    SendData(0xf7);
    SendCommand(0x20);
    WaitUntilIdle();

}

void Display1(unsigned char* frame_buffer) {
    if(count == 0){
        SendCommand(0x24);
        count++;
    }else if(count > 0 && count < 4 ){
        count++;
    }
    for(int i = 0; i < bufwidth * bufheight; i++){
            SendData(frame_buffer[i]);
    }
    if(count == 4){
        SendCommand(0x22);
        SendData(0xf7);
        SendCommand(0x20);
        WaitUntilIdle();
        count = 0;
    }
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and fast displays
parameter:
	frame_buffer : Image data
******************************************************************************/
void Display_Fast(unsigned char* frame_buffer)
{
    int w = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    int h = EPD_HEIGHT;

    if (frame_buffer != NULL) {
        SendCommand(0x24);
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                SendData(frame_buffer[i + j * w]);
            }
        }
    }

    //DISPLAY REFRESH
    SendCommand(0x22);
    SendData(0xC7);
    SendCommand(0x20);
    WaitUntilIdle();
}




/******************************************************************************
function :	Refresh a base image
parameter:
	frame_buffer : Image data	
******************************************************************************/
void DisplayPartBaseImage(unsigned char* frame_buffer)
{
    int w = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    int h = EPD_HEIGHT;

    if (frame_buffer != NULL) {
        SendCommand(0x24);
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                SendData(frame_buffer[i + j * w]);
            }
        }

        SendCommand(0x26);
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                SendData(frame_buffer[i + j * w]);
            }
        }
    }

    //DISPLAY REFRESH
    SendCommand(0x22);
    SendData(0xf7);
    SendCommand(0x20);
    WaitUntilIdle();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and partial refresh
parameter:
	frame_buffer : Image data
******************************************************************************/
void DisplayPart(unsigned char* frame_buffer)
{
    int w = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    int h = EPD_HEIGHT;

    if (frame_buffer != NULL) {
        SendCommand(0x24);
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                SendData(frame_buffer[i + j * w]);
            }
        }
    }

    //DISPLAY REFRESH
    SendCommand(0x22);
    SendData(0xff);
    SendCommand(0x20);
    WaitUntilIdle();
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void ClearPart(void)
{
    int w, h;
    w = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    h = EPD_HEIGHT;
    SendCommand(0x24);
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            SendData(0xff);
        }
    }

    //DISPLAY REFRESH
    SendCommand(0x22);
    SendData(0x0f);
    SendCommand(0x20);
    WaitUntilIdle();
}

/******************************************************************************/
void DisplayPartial(int x_start, int y_start, int x_end, int y_end, unsigned char* frame_buffer)
{
    SetWindows(x_start, y_start, x_end, y_end);
    SetCursor(x_start, y_start);

    int w = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);

    SendCommand(0x24);
    for (int j = y_start; j <= y_end; j++) {
        for (int i = (x_start / 8); i <= (x_end / 8); i++) {
            SendData(frame_buffer[i + j * w]);
        }
    }

    // Partial refresh
    SendCommand(0x22);
    SendData(0xff);
    SendCommand(0x20);
    WaitUntilIdle();
}

/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void Sleep()
{
    SendCommand(0x10); //enter deep sleep
    SendData(0x01);
    k_msleep(200);

    DigitalWrite(reset_pin, LOW);
}

/* END OF FILE */