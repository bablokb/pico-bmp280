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

add_compile_options(-DSPI_PORT=spi1)
add_compile_options(-DSPI_RX=12)
add_compile_options(-DSPI_CS=13)
add_compile_options(-DSPI_SCK=14)
add_compile_options(-DSPI_TX=15)
add_compile_options(-DALTITUDE_AT_LOC=520)
add_compile_options(-DUPDATE_INTERVAL=60)

add_compile_options(-DBMP280_DISABLE_DOUBLE_COMPENSATION)
add_compile_options(-DBMP280_DISABLE_64BIT_COMPENSATION)

pico_set_program_name(pico-bmp280 "pico-bmp280")
pico_set_program_version(pico-bmp280 "1.0")

pico_enable_stdio_uart(pico-bmp280 1)
#pico_enable_stdio_usb(pico-bmp280 1)

# Add the standard library to the build
target_link_libraries(pico-bmp280 pico_stdlib)

# Add any user requested libraries
target_link_libraries(pico-bmp280
        hardware_spi
        )

pico_add_extra_outputs(pico-bmp280)

