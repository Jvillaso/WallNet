#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "eink.h"
#include "draw.c"

int main(void)
{
        

        printk("Started\n");
        if (eink_init() != 0)
        {
                printk("Failed to initialize eink\n");
                return -1;
        }
        //Clear();

        //Reset();
        // Clear();

        // while (1) {
        //     k_sleep(K_SECONDS(5));
        // }

        // saveRawPBM("battery_0.pbm", (uint8_t*)batt[0], 48, 8);

        card_record_t austinCard;
        austinCard.valid = 1;
        strcpy(austinCard.first_name, "austin");
        strcpy(austinCard.last_name, "Lugo");
        strcpy(austinCard.last_four, "1234");
        displayCard(austinCard);

        printk("Passed card");
        char *errMsg = "err";
        displayErr(errMsg);
        // Display(frameBuffer);



        k_sleep(K_MSEC(2000)); // wait for 2 seconds
        char *msg = "Hello World";
        displayErr(msg);
        // k_busy_wait(1000000); // wait for 5 seconds
        //  displayBattery(4); // Display full battery level

        // k_busy_wait(1000000); // wait for 5 seconds
        displayBattery(0); // Display empty battery level

        return 0;
}
