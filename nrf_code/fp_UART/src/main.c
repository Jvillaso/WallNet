#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <math.h>
#include "fp_commands.h"


int main(void)
{
    fp_init();
    
    // printk("FP enrolling started!\n");
    // printk("=====================\n");
    // start_and_enroll(); //Start the process and enroll a fingerprint
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