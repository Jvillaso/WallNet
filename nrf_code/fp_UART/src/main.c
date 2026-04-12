#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <math.h>
#include "fp_commands.h"


int main(void)
{
    fp_init();
    
    printk("FP enrolling started!\n");
    printk("=====================\n");
    start_and_enroll(1, 3, true, true, true, true); //Enroll a fingerprint with ID 1, 3 entries, return status packets, allow overwriting, allow repeated registration, request to remove finger between collections
    // printk("FP identify started!\n");
    // printk("=====================\n");
    // start_and_identify(); //Start the process and identify a fingerprint



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