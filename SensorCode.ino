

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
#include <Fonts/FreeMonoBold9pt7b.h>
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

#define long SLEEP_15M = 15*60*1000000; 

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
    
  do {
    // we delay for the sensor to have proper readings
    delay(dht.getMinimumSamplingPeriod()*2);
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();
    status = dht.getStatusString();
    Serial.print(status + " ");
    Serial.print((int)temperature); Serial.print(" *C, "); 
    Serial.print((int)humidity); Serial.println(" H");
    // this loop will retry if the status string is not OK
  } while (status.indexOf("OK") != 0) ;

  // init the display
  display.init(115200);
  // 1st paint is not partial (false) and clear screen (true)
  paintRectangle(2, 4, display.width() - 4, (display.height()/2) - 8, false, true, 8, 58, String((int)temperature), true); //partial, clearscreen / true = C
  // 2nd paint is a partial (true) and not a clear (false)
  paintRectangle(2, (display.height()/2)+4, display.width() - 4, (display.height()/2) - 8, true, false, 8, 158, String((int)humidity), false);
  // we immediately turn display off to save power.
  display.powerOff();

  // sleep is set to 15m 
  int sleepTimeS = 15*60;
  // deep sleep is asking for RF to be off when waking as we do not use it.
  ESP.deepSleep(sleepTimeS * 1000000, WAKE_RF_DISABLED);
  

}

void paintRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool partial, bool clearScreen,
                    uint16_t tx, uint16_t ty, String text, bool printCvalue)
{
  display.setRotation(1);
  display.setFont(&FreeMonoBold24pt7b);
  display.setTextColor(GxEPD_BLACK);
  
  if (partial)
  {
    display.setPartialWindow(x, y, w, h);
  }
  else
  {
    display.setFullWindow();
  }
  display.firstPage();
  do
  {
    if (clearScreen) display.fillScreen(GxEPD_WHITE);
    display.fillRect(x, y, w, h, GxEPD_BLACK);
    display.fillRect(x+2, y+2, w-4, h-4, GxEPD_WHITE);    

    display.setCursor(tx, ty);

    display.print(" ");
    display.print(text);
    if (printCvalue) {
      display.print(" *C");
    } else {
      display.print("  %");
    }
  }
  while (display.nextPage());
}
