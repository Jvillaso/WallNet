#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "bq.h"

int main(void)
{
    printk("BQ25895 Test Start\n");

    BQ_init();

int level = BQ_ret_batt();

if (level >= 0) {
    
    printk("Battery level: %d\n", level);
    }

     k_sleep(K_SECONDS(1));
    

    return 0;
}