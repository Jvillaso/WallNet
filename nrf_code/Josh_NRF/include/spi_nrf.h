/**
 * 
 */

 #ifndef __SPI_H_
 #define __SPI_H_

 #include <stdint.h>
 #include "nrf.h"

 #define SCK_PIN 0
 #define MOSI_PIN 0

 void spi_init(void);
 void spi_disable(void);
 void spi_write(uint8_t * data, int len);


 #endif