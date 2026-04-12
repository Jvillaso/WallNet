#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <math.h>


#define FP_UART     DEVICE_DT_GET(DT_NODELABEL(uart0))
#define BYTE_MAX_LEN 100 //Max number of bytes in a single message
#define MAX_BACKLOG 20 //Max number of messages to keep in backlog

//Packet types
#define PKT_CMD 0x01
#define PKT_ACK 0x07

//Common commands
#define CMD_WORK_MODE 0xD3
#define CMD_SET_WRK_MODE 0xD2
#define CMD_WAKE 0xD4
#define CMD_EMPTY 0x0D
#define CMD_ENROLL 0x31
#define CMD_IDENTIFY 0x32
#define CMD_ACTIVATE 0xD4


typedef struct Response_Data { //packet data structure to hold number of packets received and the last packet received
    uint16_t num_pckts;
    char last_payload[BYTE_MAX_LEN];
    uint16_t last_payload_len;
} Response_Data; 


//Function declarations:
void fp_init();
Response_Data send_and_receive(const struct device *uart, const uint8_t *data, size_t len, uint16_t delay, uint16_t timeout);
Response_Data send_cmd(char command, char* params, uint8_t params_len, uint16_t delay);

Response_Data wake();
Response_Data get_work_mode();
Response_Data set_work_mode(uint8_t mode);
Response_Data empty();
Response_Data activate();
Response_Data enroll(uint16_t ident, uint8_t numEntries, bool status, bool overwrite, bool repeat, bool remove);
Response_Data identify(uint8_t security, uint16_t ident, bool status);
void print_packet(char* packet, uint16_t len);
bool start_and_enroll(uint16_t ident, uint8_t numEntries, bool status, bool overwrite, bool repeat, bool remove);
bool start_and_identify();
