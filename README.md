# V2 - Bosch Sensortech BME280

ESP8266, e-Paper display and a BME280 sensor make a nice outdoors unit
This simple DIY hack uses the following:

* 1 x ESP8266 for the brains (WIFI disabled via deep sleep)
* 1 x BME280 for the temperature and humidity measurements
* 1 x 1.54" e-Paper display from [Waveshare](https://www.waveshare.com/product/modules/oleds-lcds/e-paper/1.54inch-e-paper-module-b.htm)
* 1 x 16850 (3.7V) plus TP4056/protection for power-source

## Strategy
To make this work for a long time (expected to last more than ~~1 yr~~ a few weeks) the e-Paper display is used only for a few seconds each time. The ESP boots up, starts the BME280 library, reads (retry if necessary) the sensor to receive temperature and humidity. Finally, it powers on the e-Paper display and does 1 full write (required) and 1 partial write, drawing  temperature/humidity readings.

It then proceeds to go to deep sleep, making sure the wifi stays off next boot. Hopefully this will make it work.

## Added features

### TP4056
Adding a TP4056 with a 3.7V Li-Ion battery allows easier recharge. So far, the battery has worked for more than 15 days without a recharge, this is promising. I'm using the TP4056 from Amazon ([link](https://www.amazon.de/gp/product/B073JW1RR4/ref=ox_sc_act_title_1?smid=AHM5CAEOX2A0T&psc=1))

### Sensors on BME280
The BME also has the following sensors: Barometer (in hPa by default) and altitude calculation (should be accurate if the hPa provided at sea level on your area are the correct ones.)

