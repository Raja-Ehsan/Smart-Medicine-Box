#include<Arduino.h>
#include<WiFi.h>
#include "time.h"
#define Wifi_net "AndroidAP"
#define Wifi_pass "rajaamjad"
#define Timeout 20000

String time_str;

//ntpserver to get time and date without use of any external module
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;//your GMT offset (seconds)
const int   daylightOffset_sec = 3600;// your daylight offset (seconds)

void connect_wifi(){
  Serial.print("Connecting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(Wifi_net,Wifi_pass);
  //timeout to stop checking for wifi connection after 20 seconds
  unsigned long startattmpt=millis();
  //to make sure were still within our timeout we will subtract startattempttime from current uptime and makesure its less then timeout
  while(WiFi.status()!= WL_CONNECTED && millis()-startattmpt < Timeout)
  {
    Serial.print(".");
    delay(100);
  }

  if(WiFi.status()!= WL_CONNECTED)
  {
    Serial.print("Failed");
    //take any action
  }
  else
  {
    Serial.print("Connected");
    Serial.println(WiFi.localIP());
  } 

}
//function to return current local time
String printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "Time Error";
  }
  char output[80];
  strftime(output, 80, "%H:%M:%S", &timeinfo);
  time_str = String(output);
  return String(output);
}



void setup(){
  Serial.begin(115200);
  
  pinMode(18,OUTPUT);
  
  pinMode(ir,INPUT);
  connect_wifi();

   //init and get the time
   configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
   printLocalTime();

   //disconnect WiFi as it's no longer needed
   WiFi.disconnect(true);
   WiFi.mode(WIFI_OFF);
}


void loop() {
  //displays time after every seconds
  delay(1000);
      Serial.println(printLocalTime());
}