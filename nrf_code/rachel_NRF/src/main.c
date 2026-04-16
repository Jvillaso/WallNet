#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "wallnet_rfid.h"

int main(void)
{
    int ret;

    printk("=== RFID TEST START ===\n");

    ret = wallnet_rfid_init();
    i2c_scan();
    if (ret != 0) {
        printk("RFID init failed: %d\n", ret);
        return 0;
    }

    printk("RFID init OK\n");

    uint8_t cc[4] = {
    0xE1,
    0x40,
    0x0C,
    0x00
    };
    st25dv_write_bytes(0x0000, cc, 4);

    ret = wallnet_rfid_write_url("rachelchen22.github.io/#xjXWxqw7+DNXkR2HLfrfjXt+dgIoKGOc5ZitMSNKFbg82qtBi1TscZM/Ys46784s");

    // uint8_t ndef[] = {
    //     0x03, 
    //     0x0F,

    //     0xD1,
    //     0x01,
    //     0x0B,
    //     0x55,
    //     0x00,
    //     'h', 'e', 'l', 'l', 'o','.', 'c', 'o', 'm',

    //     0xFE
    // };

    // ret = st25dv_write_bytes(0x0004, ndef, sizeof(ndef));

    if (ret == 0) {
        printk("i think we wrote something?\n");
    } else {
        printk("you are a chud: error %d\n", ret);
    }

    while (1) {
        k_sleep(K_SECONDS(1));
    }
}