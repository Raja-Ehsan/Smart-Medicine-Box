#include<Arduino.h>
int ir=19;
int led=18;
void setup(){
  pinMode(ir,INPUT);
  pinMode(led,OUTPUT);
  Serial.begin(115200);
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