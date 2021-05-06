// --------------------------------------------------------------------------
// Read BMP280 sensor values with a Raspberry Pi Pico using the official Bosch-API
//
// Author: Bernhard Bablok
//
// https://github.com/bablokb/pico-bmp280
// --------------------------------------------------------------------------

#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "user.h"                             // functions needed by Bosch-API
#include "bmp280.h"                           // declaration of Bosch-API

// ---------------------------------------------------------------------------
// hardware-specific intialization
// SPI_* constants from CMakeLists.txt or user.h

void init_hw() {
  stdio_init_all();
  spi_init(SPI_PORT, 1000000);                // SPI with 1Mhz
  gpio_set_function(SPI_RX, GPIO_FUNC_SPI);
  gpio_set_function(SPI_SCK,GPIO_FUNC_SPI);
  gpio_set_function(SPI_TX, GPIO_FUNC_SPI);

  gpio_init(SPI_CS);
  gpio_set_dir(SPI_CS, GPIO_OUT);
  gpio_put(SPI_CS, 1);                        // Chip select is active-low
}

// ---------------------------------------------------------------------------
// initialize sensor

int8_t init_sensor(struct bmp280_dev *dev) {
  int8_t rslt = BMP280_OK;
  struct bmp280_config conf;

  // give sensor time to startup
  user_delay_ms(5);                  // datasheet: 2ms

  // basic initialization
  dev->dev_id   = SPI_CS;
  dev->intf     = BMP280_SPI_INTF;
  dev->read     = user_spi_read;
  dev->write    = user_spi_write;
  dev->delay_ms = user_delay_ms;
  rslt = bmp280_init(dev);
  if (rslt != BMP280_OK) {
    return rslt;
  }

  // read configuration ...
  rslt = bmp280_get_config(&conf,dev);
  if (rslt != BMP280_OK) {
    return rslt;
  }
  // ... and update it
  conf.filter  = BMP280_FILTER_COEFF_2;
  conf.os_temp = BMP280_OS_4X;
  conf.os_pres = BMP280_OS_4X;
  conf.odr     = BMP280_ODR_1000_MS;
  rslt = bmp280_set_config(&conf,dev);
  if (rslt != BMP280_OK) {
    return rslt;
  }

  // set power mode
  rslt = bmp280_set_power_mode(BMP280_NORMAL_MODE,dev);
  return rslt;
}

// ---------------------------------------------------------------------------
// read and convert sensor values

int8_t read_sensor(struct bmp280_dev *dev, float *temp, float *press) {
  int8_t rslt = BMP280_OK;
  int32_t temp32, press32;
  struct bmp280_uncomp_data raw_data;

  rslt = bmp280_get_uncomp_data(&raw_data,dev);
  if (rslt != BMP280_OK) {
    return rslt;
  }

  rslt = bmp280_get_comp_temp_32bit(&temp32,raw_data.uncomp_temp,dev);
  if (rslt != BMP280_OK) {
    return rslt;
  }
  *temp = 0.01f*temp32;

  rslt = bmp280_get_comp_pres_32bit(&press32,raw_data.uncomp_press,dev);
  *press = 0.01f*press32;
  return rslt;
}

// ---------------------------------------------------------------------------
// main loop: read data and print data to console

int main() {
  struct bmp280_dev dev;
  int8_t rslt;
  float temp, press;

  float alt_fac = pow(1.0-ALTITUDE_AT_LOC/44330.0, 5.255);

  init_hw();
  rslt = init_sensor(&dev);  
  if (rslt != BMP280_OK) {
    printf("could not initialize sensor. RC: %d\n", rslt);
  } else {
    printf("Temperature, Pressure\n");
    while (read_sensor(&dev,&temp,&press) == BMP280_OK) {
      printf("%0.2lf deg C, %0.2lf hPa\n", temp, press/alt_fac);
      sleep_ms(1000*UPDATE_INTERVAL);
    }
    printf("error while reading sensor: RC: %d", rslt);
  }
  return 0;
}
