/** TODO:
 *  Everything TBH
 *  We need to set hardware registers
 *  
 */

 #include <stdint.h>
 #include "i2c_nrf.h"
 #include "nrf.h"

 #define BQ_ADDR 0x6A

 void BQ_init(void){
    //Dont need to do much
    BQ_write(0x02, 0x3C); //Disable D+ D- lines
    BQ_write(0x04, 0x08); //Set Charge limit to 512 mA
 }

 int BQ_status(){
   uint8_t data = 0x00;
   int err = BQ_read(0x0B, &data);
   if (err) return -1;

   //TODO, add print strings here
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

 int BQ_ret_batt(){
   uint8_t temp = 0x00;
   uint8_t regi = 0x0E; //TODO: Fix this
   int err = I2C_read(BQ_ADDR, regi, &temp, 1);

   if (err) return -1;

   //else, we check battery percentage
 }