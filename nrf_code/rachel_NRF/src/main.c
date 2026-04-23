#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "bq.h"

int main(void)
{
    printk("BQ25895 Test Start\n");

    BQ_init();

    while (1)
    {
        int status = BQ_status();
        int batt   = BQ_ret_batt();

        if (status < 0)
        {
            printk("Status read error\n");
        }

        if (batt < 0)
        {
            printk("Battery read error\n");
        }
        else
        {
            printk("Battery Level: %d/3\n", batt);
        }

        k_sleep(K_SECONDS(1));
    }

    return 0;
}