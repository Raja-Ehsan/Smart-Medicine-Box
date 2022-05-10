#include<Arduino.h>
#include<WiFi.h>
#include "time.h"
#define Wifi_net "AndroidAP"
#define Wifi_pass "rajaamjad"
#define Timeout 20000

String time_str;
int ir=19;

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
    //take any action::::will add resetting automatically option later
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

//function to check whether current time is equal to specified alarm time
bool checktime(){
  //giving flexibility of around 3 seconds
  if(printLocalTime()=="16:02:00" || printLocalTime()=="16:02:01" ||printLocalTime()=="16:02:03" )
  return true;
  else return false;
}

//function to start alarm::::no buzzer for now just using an LED in place
void alarm(){

    digitalWrite(18,HIGH);
    //also not turning off led will do it next
}
void loop() {
  delay(1000);
      Serial.println(printLocalTime());
      //if check time return true if time matches then alarm
  if(checktime())
  {
  alarm();
  }
}