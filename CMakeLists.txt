## --------------------------------------------------------------------------
## Read BMP280 sensor values with a Raspberry Pi Pico using the official Bosch-API
##
## Author: Bernhard Bablok
##
## https://github.com/bablokb/pico-bmp280
## --------------------------------------------------------------------------

# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
#set(PICO_SDK_PATH "/path/to/sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico-bmp280 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(pico-bmp280 pico-bmp280.c bmp280.c user.c)

set(SPI_PORT      "spi1" CACHE STRING "spi port-name")
set(SPI_CS          "13" CACHE STRING "CS  pin number")
set(SPI_RX          "12" CACHE STRING "RX  pin number")
set(SPI_TX          "15" CACHE STRING "TX  pin number")
set(SPI_SCK         "14" CACHE STRING "SCK pin number")
set(ALTITUDE_AT_LOC  520 CACHE INT    "altitude at current location")

add_compile_definitions(SPI_PORT=${SPI_PORT})
add_compile_definitions(SPI_CS=${SPI_CS})
add_compile_definitions(SPI_RX=${SPI_RX})
add_compile_definitions(SPI_TX=${SPI_TX})
add_compile_definitions(SPI_SCK=${SPI_SCK})
add_compile_definitions(ALTITUDE_AT_LOC=${ALTITUDE_AT_LOC})
add_compile_definitions(BMP280_DISABLE_DOUBLE_COMPENSATION)
add_compile_definitions(BMP280_DISABLE_64BIT_COMPENSATION)

# library configuration (override if necessary, see pico-st7735 for a complete list)
set(SPI_TFT_PORT "spi1" CACHE STRING "TFT spi port-name")
set(SPI_TFT_CS   "9"    CACHE STRING "TFT CS  pin number")
set(SPI_TFT_DC   "10"   CACHE STRING "TFT DC  pin number")
set(SPI_TFT_RST  "11"   CACHE STRING "TFT RST pin number")
set(TFT_OPTIONS TFT_ENABLE_BLACK TFT_ENABLE_RESET TFT_ENABLE_TEXT
                                      TFT_ENABLE_SHAPES TFT_ENABLE_FONTS
            CACHE STRING "TFT options/functions")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_VERBOSE_MAKEFILE 1)
  add_compile_definitions(DEBUG)
  add_compile_definitions(UPDATE_INTERVAL=10)
else()
  add_compile_definitions(UPDATE_INTERVAL=60)
endif()

pico_set_program_name(pico-bmp280 "pico-bmp280")
pico_set_program_version(pico-bmp280 "1.0")

pico_enable_stdio_uart(pico-bmp280 1)
#pico_enable_stdio_usb(pico-bmp280 1)

# build the st7735-library from our submodule
add_subdirectory(pico-st7735)

# Add the standard library to the build
target_link_libraries(pico-bmp280 pico_stdlib
                                  st7735
                                  hardware_spi)

pico_add_extra_outputs(pico-bmp280)

