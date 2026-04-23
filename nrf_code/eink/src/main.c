#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "eink.h"
#include "draw.c"


int main(void)
{
        
        
        printk("Started\n");
        if (eink_init() != 0) {
                printk("Failed to initialize eink\n");
                return -1;
        }

        // drawCentered("   12345");
        // drawLarge("WOW letters and numbers only 12345");
        //draw("hello world");

        card_record_t austinCard;
        //card_record_t numba2;
        austinCard.valid = 1;
        strcpy(austinCard.first_name, "joshua");
        strcpy(austinCard.last_name, "Lugo");
        strcpy(austinCard.last_four, "1234");

        //drawScaled("0123456789 hello my beautiful world");
        displayCard(austinCard);
        
        //k_busy_wait(5000000);
        //Clear();

        //Init(FULL_MODE);
       
        char* errMsg = "err";
        displayErr(errMsg, strlen(errMsg));
        //Display(frameBuffer);

        // //Init(FULL_MODE);
        k_busy_wait(5000000); // wait for 5 seconds
        displayCard(austinCard);
        


        return 0;
}
