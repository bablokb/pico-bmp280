// --------------------------------------------------------------------------
// Declaration of user-supplied functions for Bosch-API
//
// Author: Bernhard Bablok
//
// https://github.com/bablokb/pico-bmp280
// --------------------------------------------------------------------------

#ifndef _USER_H
#define _USER_H

#include "pico/stdlib.h"

// --------------------------------------------------------------------------
// constants (set in CMakeLists.txt)

// SPI pin-numbers
#ifndef SPI_PORT
  #define SPI_PORT spi1
#endif
#ifndef SPI_RX
  #define SPI_RX  12
#endif
#ifndef SPI_CS
  #define SPI_CS  13
#endif
#ifndef SPI_SCK
  #define SPI_SCK 14
#endif
#ifndef SPI_TX
  #define SPI_TX  15
#endif

// current altitude in meters
#ifndef ALTITUDE_AT_LOC
  #define ALTITUDE_AT_LOC 520
#endif

// update interval
#ifndef UPDATE_INTERVAL
  #define UPDATE_INTERVAL 60
#endif

// --------------------------------------------------------------------------
// user-supplied functions

void user_delay_ms(uint32_t period);
int8_t user_spi_read(uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
int8_t user_spi_write(uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);

#endif
