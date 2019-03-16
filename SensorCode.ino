

// Display Library example for SPI e-paper panels from Dalian Good Display and boards from Waveshare.
// Requires HW SPI and Adafruit_GFX. Caution: these e-papers require 3.3V supply AND data lines!
//
// Display Library based on Demo Example from Good Display: http://www.good-display.com/download_list/downloadcategoryid=34&isMode=false.html
//
// Author: Jean-Marc Zingg
//
// Version: see library.properties
//
// Library: https://github.com/ZinggJM/GxEPD2

// base class GxEPD2_GFX can be used to pass references or pointers to the display instance as parameter, uses ~1.2k more code
// enable or disable GxEPD2_GFX base class
#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

#include <Fonts/FreeMonoBold24pt7b.h>

#if defined (ESP8266)
// select one and adapt to your mapping, can use full buffer size (full HEIGHT)
GxEPD2_BW<GxEPD2_154, GxEPD2_154::HEIGHT> display(GxEPD2_154(/*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4));
#endif


// #include "GxEPD2_boards_added.h"



#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif


#define SLEEP_15M   15*60*1000000
ADC_MODE(ADC_VCC);


// ----------------------------------------------------------------------------
// I2C setup: SDA and SCL pin selection
// ----------------------------------------------------------------------------
#include <Wire.h>

#define SDA 12  // to GPIO 4 (D2) 
#define SCL 5 // to GPIO 5 (D1).


// ----------------------------------------------------------------------------
// BME280 libraries and variables
// Written by Limor Fried & Kevin Townsend for Adafruit Industries.
// https://github.com/adafruit/Adafruit_BME280_Library
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#undef BME280_ADDRESS         // Undef BME280_ADDRESS from the BME280 library to easily override I2C address
#define BME280_ADDRESS (0x76) // Low = 0x76 , High = 0x77 (default on adafruit and sparkfun BME280 modules, default for library)

Adafruit_BME280 bme; // Use I2C


#define SEALEVELPRESSURE_HPA (1013.25)

/**
   in setup, we start the serial (for debugging reasons)
   and the DHT11 library
*/
void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
#if defined(SDA) && defined(SCL)
  Serial.print(F("Using custom I2C pins: SDA on GPIO"));
  Serial.print(SDA);
  Serial.print(F(" and SCL on GPIO"));
  Serial.println(SCL);
  Wire.begin(SDA, SCL);
#else
  Serial.println(F("Using default I2C pins: SDA on GPIO4 and SCL on GPIO5"));
  Wire.begin();
#endif
  if (!bme.begin(BME280_ADDRESS)) {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring or I2C address!"));
    while (1) {
      yield();
    }
  }

  Serial.println("setup done");
}

/**
   we do a deep sleep at the end of this loop, so it is only used once.
*/
void loop()
{
  delay(500); //sleep for 0.5 for voltage to stabilize


  float humidity = bme.readHumidity();
  float temperature = bme.readTemperature();
  float alt = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float pressure = bme.readPressure() / 100.0F;


  const float adc_correction = 1.71f;
  float voltage  = ESP.getVcc() / 1000.0 + adc_correction;
  
  // init the display
  display.init(115200);
  
  display.setFullWindow();
  display.firstPage();
  display.fillScreen(GxEPD_WHITE);
  paintTempHum(2, 4, display.width() - 4, (display.height() / 2) - 8,  8, 58, temperature, true);
  paintTempHum(2, (display.height() / 2) - 16, display.width() - 4, (display.height() / 2) - 8,  8, 142, humidity, false);
  printVoltage(2, display.height() - 32, voltage, pressure, alt);
  display.nextPage();
  // we immediately turn display off to save power.
  display.powerOff();


  // deep sleep is asking for RF to be off when waking as we do not use it.
  ESP.deepSleep(SLEEP_15M, WAKE_RF_DISABLED);
  //delay(5000);


}

void printVoltage(uint16_t x, uint16_t y, float voltage, float press, float alt) {

  display.setFont(&FreeSans9pt7b);
  display.setCursor(x, y + 6);

  display.print(voltage, 2);
  if (voltage < 3.1) {
    display.print("v");
  } else {
    display.print("V");
  }
  display.setCursor(x, y + 24);

  display.print(" ");
  display.print(press,1);
  display.print("hPa ");
  display.print(alt,1);
  display.print("m");
}

void paintBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {

  display.fillRect(x, y, w, h, GxEPD_BLACK);
  display.fillRect(x + 2, y + 2, w - 4, h - 4, GxEPD_WHITE);

}

/**
   custom paint message function, has x,y,w,h for a rectangle, option to do a partial or full screen refresh,
   option to do a clear screen and print either *C or % for the temp/humidity value.
*/
void paintTempHum(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                  uint16_t tx, uint16_t ty, float value, bool printCvalue)
{
  display.setRotation(1);
  display.setTextColor(GxEPD_BLACK);
  //paintBox(x,y,w,h);

  display.setFont(&FreeSansBold9pt7b);
  display.setCursor(x, y + 10);
  if (printCvalue) {
    display.print("Temperature");
  } else {
    display.print("Humidity");
  }


  display.setFont(&FreeMonoBold24pt7b);
  display.setCursor(tx, ty);

  display.print(value, 1);
  if (printCvalue) {
    display.print("C");
  } else {
    display.print("%");
  }
}
