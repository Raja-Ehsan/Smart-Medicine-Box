#include<Arduino.h>
#include<WiFi.h>

#define Wifi_net "eduroam"
#define Wifi_pass "*******"
#define Timeout 20000

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
void setup(){
  Serial.begin(115200);
  connect_wifi();
}

void loop()
{
  int irvalue=digitalRead(ir);
  if(irvalue==LOW){
    digitalWrite(led,HIGH);
  }
  else if(irvalue==HIGH){
    
    digitalWrite(led,LOW);
  }
}