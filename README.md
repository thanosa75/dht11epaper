# dht11epaper

ESP8266, e-Paper display and a DHT 11 sensor make a nice outdoors unit
This simple DIY hack uses the following:

* 1 x ESP8266 for the brains (WIFI disabled via deep sleep)
* 1 x DHT11 (blue) for the temperature and humidity measurements
* 1 x 1.54" e-Paper display from [Waveshare](https://www.waveshare.com/product/modules/oleds-lcds/e-paper/1.54inch-e-paper-module-b.htm)
* 3 x AA (1.5V) batteries for power-source

## Strategy
To make this work for a long time (expected to last more than 1 yr) the e-Paper display is used only for a few seconds each time. The ESP boots up, starts the DHT library, reads (retry if necessary) the sensor to receive temperature and humidity. Finally, it powers on the e-Paper display and does 1 full write (required) and 1 partial write, drawing two rectangles and the temperature/humidity readings.

It then proceeds to go to deep sleep, making sure the wifi stays off next boot. Hopefully this will make it work.

## Next steps
Use the `ESP.getVcc()` functionality to read available VCC and signal a low-battery case. Currently, the sensor can be seen operating every 15m by the screen refresh (not very user friendly.)

