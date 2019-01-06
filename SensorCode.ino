

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

#include "DHTesp.h"

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

DHTesp dht;

#define SLEEP_20M   20*60*1000000  
ADC_MODE(ADC_VCC);


/**
 * in setup, we start the serial (for debugging reasons) 
 * and the DHT11 library
 */
void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  
  dht.setup(12, DHTesp::DHT11); // Connect DHT sensor to GPIO 

  Serial.println("setup done");
}

/**
 * we do a deep sleep at the end of this loop, so it is only used once.
 */
void loop()
{
  float humidity = 0.0;
  float temperature = 0.0;
  String status = dht.getStatusString();

  delay(500); //sleep for 0.5 for voltage to stabilize
  float voltage=0.00f;
  const float adc_correction = 0.00f;
  voltage  = ESP.getVcc()/1000.0+adc_correction;
  Serial.print(voltage);
  Serial.println(" V");    
  do {
    // we delay for the sensor to have proper readings
    delay(dht.getMinimumSamplingPeriod()*2);
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();
    status = dht.getStatusString();
    Serial.println(status + " ");
    Serial.print((int)temperature); Serial.print(" *C, "); 
    Serial.print((int)humidity); Serial.println(" H");
    // this loop will retry if the status string is not OK
  } while (status.indexOf("OK") != 0) ;

  // init the display
  display.init(115200);
  display.setFullWindow();
  display.firstPage();
  display.fillScreen(GxEPD_WHITE);
  paintTempHum(2, 4, display.width() - 4, (display.height()/2) - 8,  8, 58, String((int)temperature), true); 
  paintTempHum(2, (display.height()/2)+4, display.width() - 4, (display.height()/2) - 8,  8, 158, String((int)humidity), false);
  printVoltage(2, display.height()-16, voltage);
  display.nextPage();
  // we immediately turn display off to save power.
  display.powerOff();


  // deep sleep is asking for RF to be off when waking as we do not use it.
  ESP.deepSleep(SLEEP_20M, WAKE_RF_DISABLED);
  //delay(5000);
  

}

void printVoltage(uint16_t x, uint16_t y, float voltage) {
  
  display.setFont(&FreeSans9pt7b);
  display.setCursor(x,y+6);
  display.print("VCC: ");
  display.print(voltage);
  display.print("V");

  if (voltage < 2.6) {
    display.print("  [LOW]");
  }
}

void paintBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h){

    display.fillRect(x, y, w, h, GxEPD_BLACK);
    display.fillRect(x+2, y+2, w-4, h-4, GxEPD_WHITE);    

}

/**
 * custom paint message function, has x,y,w,h for a rectangle, option to do a partial or full screen refresh,
 * option to do a clear screen and print either *C or % for the temp/humidity value.
 */
void paintTempHum(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                    uint16_t tx, uint16_t ty, String text, bool printCvalue)
{
  display.setRotation(1);
  display.setTextColor(GxEPD_BLACK);
  //paintBox(x,y,w,h);
  
  display.setFont(&FreeSansBold9pt7b);
  display.setCursor(x,y+10);
  if (printCvalue) {
    display.print("Temperature");
  } else {
    display.print("Humidity");
  }

  
  display.setFont(&FreeMonoBold24pt7b);
  display.setCursor(tx, ty);

  display.print("   ");
  display.print(text);
  if (printCvalue) {
    display.print("C");
  } else {
    display.print("%");
  }
}
