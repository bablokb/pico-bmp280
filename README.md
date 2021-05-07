pico-bmp280
===========

This is an example program that reads BMP280 sensor values with a Raspberry Pi Pico using the official Bosch-API (SPI-interface).

Note that about 99% of the sellers offer a "BME280/BMP280"-breakout, but instead of
a BME280 (chip-id 0x60), you will most probably receive a cheaper BMP280 (chip-id 0x56, 0x57 or 0x58). The former provides readouts for temperature, pressure and humidity, while the latter lacks humidity.

You will find a similar project for the BME280 in
<https://github.com/bablokb/pico-bme280>.


License
-------

My code is licensed using the GPL v3, but it uses the files from Bosch-Sensortec
<https://github.com/BoschSensortec/BMP280_driver>, which is licensed under BSD-3.

