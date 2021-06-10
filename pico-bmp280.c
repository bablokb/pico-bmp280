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

#include "ST7735_TFT.h"
#include "FreeMonoOblique12pt7b.h"

#define FIELDS     2
#define FIELD_W  110
#define FIELD_H   36
#define FIELD_R    5
#define TEXT_X     8
#define TEXT_Y    26
#define TEXT_FG   ST7735_BLACK
#define TEXT_BG   ST7735_WHITE
#define TFT_BG    ST7735_BLUE

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

  gpio_init(SPI_TFT_CS);
  gpio_set_dir(SPI_TFT_CS, GPIO_OUT);
  gpio_put(SPI_TFT_CS, 1);                        // Chip select is active-low

  gpio_init(SPI_TFT_DC);
  gpio_set_dir(SPI_TFT_DC, GPIO_OUT);
  gpio_put(SPI_TFT_DC, 0);                        // Chip select is active-low

  gpio_init(SPI_TFT_RST);
  gpio_set_dir(SPI_TFT_RST, GPIO_OUT);
  gpio_put(SPI_TFT_RST, 0);
}

// ---------------------------------------------------------------------------
// initialize TFT

void init_tft() {
  #ifdef DEBUG
    printf("[DEBUG] initializing TFT\n");
  #endif
  TFT_BlackTab_Initialize();
  fillScreen(TFT_BG);
  setFont(&FreeMonoOblique12pt7b);
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
#ifdef DEBUG
  printf("[DEBUG] chip-id: 0x%x\n",dev->chip_id);
#endif
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
// display sensor data on TFT

void display_data(float temp, float press) {
  char values[2][10];

  snprintf(values[0],10,"%+0.1f°C",temp);
  snprintf(values[1],10,"%0.0fhPa",press);

  // clear output area
  uint8_t hgap = (tft_width-FIELD_W)/2;
  uint8_t vgap = (tft_height-FIELDS*FIELD_H)/(FIELDS+1);
  uint8_t y    = vgap;

  for (uint8_t i=0; i<FIELDS; ++i) {
    fillRoundRect(hgap,y,FIELD_W,FIELD_H,FIELD_R,TEXT_BG);
    y += FIELD_H+vgap;
  }

  // write sensor readouts
  y = vgap + TEXT_Y;
  for (uint8_t i=0; i<FIELDS; ++i) {
    drawText(hgap+TEXT_X,y,values[i],TEXT_FG,TEXT_BG,1);
    y += FIELD_H+vgap;
  }
}

// ---------------------------------------------------------------------------
// main loop: read data and print data to console

int main() {
  struct bmp280_dev dev;
  int8_t rslt;
  float temp, press;

  float alt_fac = pow(1.0-ALTITUDE_AT_LOC/44330.0, 5.255);

  init_hw();
  init_tft();
  rslt = init_sensor(&dev);
  if (rslt != BMP280_OK) {
    printf("could not initialize sensor. RC: %d\n", rslt);
  } else {
    #ifdef DEBUG
      printf("Temperature, Pressure\n");
    #endif
    sleep_ms(20);
    while (read_sensor(&dev,&temp,&press) == BMP280_OK) {
      press /= alt_fac;
      #ifdef DEBUG
        printf("%0.1lf °C, %0.0lf hPa\n", temp, press);
      #endif
      display_data(temp,press);
      sleep_ms(1000*UPDATE_INTERVAL);
    }
    printf("error while reading sensor: RC: %d", rslt);
  }
  return 0;
}
