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

static const struct device *fp_uart = FP_UART; //Get the UART device from the device tree

//Reception variables
static char msg_backlog[MAX_BACKLOG][BYTE_MAX_LEN]; //Backlog of received messages
static uint16_t backlog_sizes[MAX_BACKLOG]; //Sizes of messages in backlog
static uint8_t backlog_size = 0; //Index for the message backlog
static char read_buf[BYTE_MAX_LEN];
static uint16_t bytes_read = 0;
static uint16_t bytes_to_read = 9;
static bool response_flag = false; //Flag to indicate when a response packet is received 

typedef struct Response_Data { //packet data structure to hold number of packets received and the last packet received
    uint16_t num_pckts;
    char last_payload[BYTE_MAX_LEN];
    uint16_t last_payload_len;
} Response_Data; 

//Transmission variables
static const uint8_t *tx_buf;
static size_t tx_len;
static size_t tx_pos;



//Function declarations:
void uart_send_bytes_irq(const struct device *uart, const uint8_t *data, size_t len);
void free_backlog_entry(uint8_t index);

static void uart_cb(const struct device *dev, void *user_data)
{
    //UART interrupt handler

    if (!uart_irq_update(dev)) return; //Check if interrupt is valid, if not exit handler

    //printk("UART callback triggered\n");

    static const uint8_t header[] = { //Header all packets should have
        0xef, 0x01, 0xff, 0xff, 0xff, 0xff, 
    };

    uint8_t c;
    uint8_t ret;

    // Handle receiving data
    if(uart_irq_rx_ready(dev)){
        //printk("Reading data\n");
        while ((ret = uart_fifo_read(dev, &c, 1)) == 1 || bytes_read < bytes_to_read) { //Loop until we read all bytes
            
            if(ret == 1) { //Only add to buffer if succesfully read a byte 
                //printk("%X\n", c);
                read_buf[bytes_read++] = c; //Add to buffer
                
                if(bytes_read == 9) { //Check if read the first 9 bytes (header + length) to determine how many bytes to read total
                    
                    //Check if header is correct
                    if(memcmp(read_buf, header, sizeof(header)) == 0) {
                        //printk("Header matched!\n");

                        //Update length of packet
                        int16_t length_of_pckt = (read_buf[7] << 8) | (read_buf[8]); //7th and 8th byte have the length
                        //printk("Pckt len: %d\n", length_of_pckt);
                        bytes_to_read += length_of_pckt; //Update max bytes to read
                        //printk("Expecting %d bytes total\n", bytes_to_read);
                    } 
                    else { //Incorrect header, reset and exit handler
                        printk("Header mismatch\n");
                        //TODO: handle this?
                        bytes_read = 0; //Reset buffer for next packet
                        bytes_to_read = 9; //Reset expected bytes for next packet 
                        return;
                    }
                } 
            }     
        }  
        

        //print out buffer
        // printk("Received packet: ");
        // for(int i = 0; i < bytes_read; i++) {   
        //     printk("%X ", read_buf[i]);
        // }
        // printk("\n");

        //Add PAYLOAD to backlog
        backlog_sizes[backlog_size] = bytes_read - 11; //Store size of message in backlog sizes array
        memcpy(msg_backlog[backlog_size], &read_buf[9], backlog_sizes[backlog_size]); //Copy message to backlog
        backlog_size += 1; //Update backlog index

        if (backlog_size >= MAX_BACKLOG) {
            printk("Backlog full!!!\n");
            //TODO:????
        }

        bytes_read = 0; //Reset buffer for next packet
        bytes_to_read = 9; //Reset expected bytes for next packet 
        response_flag = true; //Set flag to indicate we got a response packet

    }

    //Handle sending data
    if(uart_irq_tx_ready(dev)){

        printk("Transmitting data: ");
        for(int i = 0; i < tx_len; i++) {
            printk("%X ", tx_buf[i]);
        }
        printk("\n");
        
    
        //Below is taken from ChatGPT w/ prompt "how to send bytes with the zephyr libraries"
        int sent = uart_fifo_fill(dev,
                                &tx_buf[tx_pos],
                                tx_len - tx_pos);
        tx_pos += sent;

        if (tx_pos >= tx_len) {
            uart_irq_tx_disable(dev);
        }
    }
    
}

void uart_send_bytes_irq(const struct device *uart, const uint8_t *data, size_t len){
    //Below is taken from ChatGPT w/ prompt "how to send bytes with the zephyr libraries"

    tx_buf = data;
    tx_len = len;
    tx_pos = 0;

    uart_irq_tx_enable(uart); //Enable interrupt
}

bool wait_for_resp(uint16_t delay, uint16_t timeout) {
    //Waits for a response flag
    //delay in ms, timeout in ms

    uart_irq_rx_enable(fp_uart); //Enable receiving
    uint64_t count = 0;
    uint64_t ceiling = ceil(timeout / delay);

    while (!response_flag) {
        k_sleep(K_MSEC(delay));
        if (count >= ceiling) {
            return false; // Timeout occurred
        }
        count++;
    }

    response_flag = false; //Reset flag for next packet

    uart_irq_rx_disable(fp_uart); //disable receiving

    return true; //Response received
}

Response_Data send_and_receive(const struct device *uart, const uint8_t *data, size_t len, uint16_t delay, uint16_t timeout) {

    uart_send_bytes_irq(uart, data, len); //send data
    
    wait_for_resp(delay, timeout); //Wait for response

    Response_Data resp;
    resp.num_pckts = backlog_size; //Number of packets received 
    resp.last_payload_len = backlog_sizes[backlog_size - 1]; //Size of last packet received
    if(backlog_size > 0) {
        memcpy(resp.last_payload, msg_backlog[backlog_size - 1], resp.last_payload_len); //Copy last packet received to response data
    }
    else {
        memset(resp.last_payload, 0, BYTE_MAX_LEN); //If no packets received, set last packet to all 0s
    }

    //TODO: checksum

    return resp; 
}

void free_entire_backlog() {
    //Free memory allocated for backlog messages

    for(uint8_t i = 0; i < MAX_BACKLOG; i++) {
        free_backlog_entry(i);
    }

    backlog_size = 0; //Reset backlog size
}

void free_backlog_entry(uint8_t index) {
    //Free memory allocated for a specific backlog message

    char zeros[BYTE_MAX_LEN] = {0}; //Array of 0s to overwrite message with 
    memcpy(msg_backlog[index], zeros, BYTE_MAX_LEN); //Overwrite message with 0s
    backlog_sizes[index] = 0; //Reset size of message in backlog sizes array
}

uint16_t checksum(char* data, uint8_t start, uint8_t len) {
    //Calculate checksum for a given array of data and its length
    uint32_t sum = 0;
    for(uint16_t i = start; i < len; i++) {
        sum += data[i];
    }

    return (uint16_t) (sum & 0xFFFF); //Return least significant 2 bytes of sum as checksum
}


Response_Data send_cmd(char command, char* params, uint8_t params_len, uint16_t delay) {
    //Helper function to send a command with parameters to the FP scanner
    //Command is a single byte, params is an array of bytes, params_len is the length of the params array

    if(backlog_size > 0) {
        free_entire_backlog(); //Free backlog of messages before sending command to ensure we only have the response to this command in the backlog
    }
    

    uint16_t payload_len = params_len + 3;
    uint16_t packet_len = payload_len + 9; //Payload + header (6 bytes) + length (2 bytes) + pckt type (1 byte)
    char packet[BYTE_MAX_LEN];
    uint16_t packet_iter = 0;
    
    //Construct packet 

    //Add header
    static const uint8_t header[] = {
        0xef, 0x01, 0xff, 0xff, 0xff, 0xff
    };
    memcpy(packet, header, sizeof(header));
    packet_iter += sizeof(header);

    //Add packet type (command)
    packet[packet_iter++] = (char) PKT_CMD; //Packet type (command)

    //Add length
    //add msb then lsb?
    packet[packet_iter++] = (payload_len >> 8) & 0xFF;  // MSB
    packet[packet_iter++] = payload_len & 0xFF;         // LSB

    //Add command
    packet[packet_iter++] = (char) command;

    //Add parameters
    if (params_len > 0) {
        memcpy(&packet[packet_iter], params, params_len);
        packet_iter += params_len;
    }

    //Checksum
    //add msb then lsb?
    uint16_t chksum = checksum(packet, 6, packet_iter); //Calculate checksum of payload
    packet[packet_iter++] = (chksum >> 8) & 0xFF;  // MSB
    packet[packet_iter++] = chksum & 0xFF;         // LSB

    //Send packet
    return send_and_receive(fp_uart, (uint8_t*) packet, packet_len, delay, 5000);
}

Response_Data wake() {
    return send_cmd(CMD_WAKE, NULL, 0, 10);
}
    
Response_Data get_work_mode() {
    return send_cmd(CMD_WORK_MODE, NULL, 0, 10);
}
    
Response_Data set_work_mode(uint8_t mode) {
    char* mode_str = (char*) &mode; //Convert mode to char array to send as parameters
    return send_cmd(CMD_SET_WRK_MODE, mode_str, sizeof(mode), 10); //Send command with mode as parameter
}
    
Response_Data empty() {
    return send_cmd(CMD_EMPTY, NULL, 0, 10);
}

Response_Data enroll(uint16_t ident, uint8_t numEntries, bool status, bool overwrite, bool repeat, bool remove) {
    //Sends enroll command with parameters for enrolling a fingerprint

    //ident is the ID to enroll the fingerprint under (MSB then LSB)

    //numEntries is the number of times to place finger on sensor when enrolling (2 or 4 usually),

    //status is whether to return a packet for every level the fp sensor is at,

    //overwrite is to allow overwriting an existing fingerprint with the same ID,
    //0- not allowed , 1- allowed;  

    //repeat is to allow repeated fp registration, 
    //0- allowed , 1- not allowed;

    //remove is to indicate during multiple fingerprint collections, 
    //is it required to remove the finger beforeentering the next fingerprint image collection ? 
    //0- Request to leave; 1- Do not request to leave;

    char params[5]; 

    params[0] = (char) ((ident >> 8) & 0xFF); // MSB
    params[1] = (char) (ident & 0xFF); // LSB
    params[2] = (char) (numEntries & 0xFF); //Number of entries to enroll for this fingerprint
    
    char param = 0;
    if (status) param |= (1 << 2);
    if (overwrite) param |= (1 << 3);
    if (!repeat) param |= (1 << 4);
    if (!remove) param |= (1 << 5);
        
    params[3] = (char) ((param >> 8) & 0xFF); // MSB
    params[4] = (char) (param & 0xFF); // LSB

    return send_cmd(CMD_ENROLL, params, sizeof(params), 5000);
}

Response_Data identify(uint8_t security, uint16_t ident, bool status) {
    //Sends identify command with parameters for identifying a fingerprint

    //security is the security level to use for identification (0-3, higher is more secure)

    //ident is the ID to identify against (MSB then LSB)

    //status is whether to return a packet for every level the fp sensor is at

    char params[5]; 

    params[0] = (char) (security & 0xFF); //Security level to use for identification,
    params[1] = (char) ((ident >> 8) & 0xFF); // MSB of ident
    params[2] = (char) (ident & 0xFF); // LSB of ident

    char param = 0;
    if (status) param |= (1 << 2);
    params[3] = (char) ((param >> 8) & 0xFF); // MSB
    params[4] = (char) (param & 0xFF); // LSB

    return send_cmd(CMD_IDENTIFY, params, sizeof(params), 1000);
}

void print_packet(char* packet, uint16_t len) {
    //Helper function to print a packet in hex format
    
    for(int i = 0; i < len; i++) {
        printk("%X ", packet[i]);
    }
    printk("\n");
}

void start_and_enroll() {
    //Wake, set work mode, and enroll a fingerprint 


    Response_Data resp = wake(); //Wake up FP scanner
    printk("Number of packets received: %d\n", resp.num_pckts);
    printk("Last packet received: ");
    print_packet(resp.last_payload, resp.last_payload_len);

    resp = set_work_mode(1); //Wake up FP scanner
    printk("Number of packets received: %d\n", resp.num_pckts);
    printk("Last packet received: ");
    print_packet(resp.last_payload, resp.last_payload_len);

    resp = enroll(1, 1, false, true, false, false); //Enroll a fingerprint with ID 1, 1 entry, no return status packets, allow overwriting, allow repeated registration, request to remove finger between collections
    printk("Number of packets received: %d\n", resp.num_pckts);
    printk("Last packet received: ");
    print_packet(resp.last_payload, resp.last_payload_len);
}

void start_and_identify() {
    //Wake, set work mode, and identify a fingerprint
    Response_Data resp = wake(); //Wake up FP scanner
    printk("Number of packets received: %d\n", resp.num_pckts);
    printk("Last packet received: ");
    print_packet(resp.last_payload, resp.last_payload_len);

    resp = set_work_mode(1); //Wake up FP scanner
    printk("Number of packets received: %d\n", resp.num_pckts);
    printk("Last packet received: ");
    print_packet(resp.last_payload, resp.last_payload_len);

    resp = identify(1, 1, false); //Identify a fingerprint with security level 1, ID 1, no return status packets
    printk("Number of packets received: %d\n", resp.num_pckts);
    printk("Last packet received: ");
    print_packet(resp.last_payload, resp.last_payload_len);
}

int main(void)
{
    
    if (!device_is_ready(fp_uart)) {
        printk("FP UART not ready\n");
        return -1;
    }

    uart_irq_callback_user_data_set(fp_uart, uart_cb, NULL); //Set the callback for the UART interrupts
    
    printk("FP enrolling started!\n");
    printk("=====================\n");
    start_and_enroll(); //Start the process and enroll a fingerprint
    printk("FP identify started!\n");
    printk("=====================\n");
    start_and_identify(); //Start the process and identify a fingerprint

    while (1) { //Dont exit at the end
        k_sleep(K_FOREVER);
    }
    return 0;
}



// uart_irq_rx_enable(fp_uart); //Enable receiving
// printk("FP reader started, waiting for bytes...\n");

// while (1) {
//     k_sleep(K_FOREVER);
//     // k_sleep(K_MSEC(5000));

//     // printk("SEND A RESPONSE AFTER 5 SEC\n");
//     // static const uint8_t msg[] = {
//     //     0xef, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x03, 0xd4, 0x00, 0xd8
//     // };
//     // uart_send_bytes_irq(fp_uart, msg, sizeof(msg));
// }