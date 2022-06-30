
/* 
 *  
 *  
INSTALAR LIBRERIA TinyGPSPlus

*/



#include <Arduino.h>

#include <TinyGPS++.h>
#include "heltec.h"
#define BAND    868E6 //433E6  //you can set band here directly,e.g. 868E6,915E6
TinyGPSPlus gps;
String out="no data";
int x = 0;
int count=0;
unsigned long timing;
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10    /* Time ESP32 will go to sleep (in seconds) */
RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void displayInfo();

void setup()
{
   Serial.begin(115200);
   delay(100);
  Serial2.begin(9600, SERIAL_8N1,2,17);
  
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->clear();
  Heltec.display->drawString(0, 10, "Starting...");
  Heltec.display->display();

//Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  /*
  First we configure the wake up source
  We set our ESP32 to wake up every 5 seconds
  */
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
  
    LoRa.setTxPower(20,RF_PACONFIG_PASELECT_PABOOST);
  //LoRa.setSignalBandwidth(62.5E3);
   LoRa.setSpreadingFactor(10);
  
  
  
}

void loop(){

    while (Serial2.available() > 0)
    if (gps.encode(Serial2.read())){
      
        displayInfo();
      
      
      LoRa.beginPacket();
      LoRa.print(out);
      delay(3000); 
      LoRa.endPacket();
      
      Heltec.display->clear();
      Heltec.display->drawStringMaxWidth(0, 10, 128, out);
      Heltec.display->display();
      Serial.println(out);
delay(3000);
      
    }
      
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }

 /*
count++;
if (count==10){
  count = 0;
  x =0;
 
  Serial.println("Going to sleep now");
  delay(500);
  Serial.flush(); 
  esp_deep_sleep_start();
 
  }
*/

  
}

void displayInfo()
{
    String str = "";
    char buff[12];
           
    float lat = gps.location.lat();
    dtostrf(lat, 5, 6, buff);
    str = str + buff + ","; 
    float lng = gps.location.lng();
    dtostrf(lng, 5, 6, buff);              
    str = str + buff + "  ";

    str = str + gps.time.hour() + ":";
    str = str + gps.time.minute() + ":";
    str = str + gps.time.second() + "  "; 
    
    str = str + gps.altitude.meters() + "  ";
    str = str + x;           
    out = str;
   x = x + 1;
    
}
