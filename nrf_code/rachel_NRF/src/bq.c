/** TODO:
 *  Everything TBH
 *  We need to set hardware registers
 *  
 */

#include <stdint.h>
#include "bq.h"
#include "i2c_nrf.h"
#include <zephyr/drivers/i2c.h>

#define BQ_ADDR 0x6A

void BQ_init(void){
  BQ_write(0x02, 0x3C); //Disable D+ D- lines
  BQ_write(0x04, 0x08); //Set Charge limit to 512 mA
}

/*
int BQ_status(){
  uint8_t data = 0x00;
  int err = BQ_read(0x0B, &data);
  if (err) return -1;

  //TODO, add print strings here
} */

int BQ_status(){
   uint8_t data = 0x00;
   int err = BQ_read(0x0B, &data);
   if (err) {
      printk("BQ_status: I AM A CHUD. there has been an error\r\n");
      return -1;
   }

   printk("BQ REG0B = 0x%02X\r\n", data);

   return data;
}


int BQ_write(uint8_t reg, uint8_t value){
  uint8_t buff[2];
  buff[0] = reg;
  buff[1] = value;

  return I2C_write(BQ_ADDR, buff, 2);
}

int BQ_read(uint8_t reg, uint8_t* data){
  uint8_t regi = reg;

  return I2C_read(BQ_ADDR, regi, data, 1);
}


/*
int BQ_ret_batt(){
  uint8_t temp = 0x00;
  uint8_t regi = 0x0E; //TODO: Fix this
  // on it boss
  int err = I2C_read(BQ_ADDR, regi, &temp, 1);

  if (err) return -1;

  //else, we check battery percentage
} */

int BQ_ret_batt(){
    uint8_t temp = 0x00;

    if (BQ_read(0x0E, &temp)) {
      return -1;
    }

    uint8_t val = temp & 0x7F; // uhh extract usable bits?

    // val is between 0 - 127 now (7bit) ADC

    // range is 2304 (0000000) to 4848 (1111111)

    int mv = 2304 + (val * 20); // convert to number

    // convert to 0–3

    // doing like an exponential thing is like way overkill so it should look more like
    //        \ 4.2
    //         \ 
    //          \
    //            -3.95------------------3.7----
    //                                           \
    //                                            \ 3.4
    //                                             \
    // im literally the goat at ascii art

    //     ⠀⠀⠀⠀⠀⠀ ⣠⠞⠉⠉⠉⠉⠉⠉⠙⠓⢦⣄
    // ⠀⠀⠀⠀⠀⠀⠀⠀⠀⣴⠏⢠⣤⡶⠶⠶⠶⠶⣤⣄⠀⠹⣧⡀
    // ⠀⠀⠀⠀⠀⠀⠀⠀⣸⠟⠀⠸⣿⣦⣄⣀⣠⣤⣤⡿⠀⠀⠘⣧
    // ⠀⠀⠀⠀⠀⠀⠀⢰⡟⠀⠀⠀⠀⠉⠉⠉⠉⠉⠁⠀⠀⠀⠀⢹⡄
    // ⠀⠀⠀⠀⠀⠀⠀⣼⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡇
    // ⠀⠀⠀⠀⠀⠀⢀⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⡇
    // ⠀⠀⠀⠀⠀⠀⣸⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣇
    // ⠀⠀⠀⠀⠀⢀⡿⠀⠀⠀⢠⡶⠒⠒⠒⠒⠲⣦⡀⠀⠀⠀⠀⠀⣿
    // ⠀⣀⣠⣄⣀⣼⡇⠀⠀⠀⢸⡇⠀⠀⠀⠀⠀⢸⡇⠀⠀⠀⠀⠀⡇
    // ⠸⣏⡀⠀⠀⠈⠀⠀⠀⣀⣼⡇⠀⠀⣀⣀⣠⣼⠃⠀⠀⠀⠀⠀⡏
    // ⠀⠈⠛⠛⠒⠒⠒⠛⠛⠉⠁⠀⠀⣾⠉⠀⠀⠀⠀⠀⠀⠀⢀⣰⠇
    // ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⠛⠒⠒⠒⠚⠛⠉⠉

    if (mv < 3400) { // deadge
      return 0;
    }
    else if (mv < 3700) { // flat region
      return 1;
    }
    else if (mv < 3950) { // flat region
      return 2;
    }
    else {
      return 3;
    }
}