

//----------Included Libraries---------
#include <Arduino.h>
#include "WiFi.h"
#include "time.h"
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ESP_Mail_Client.h>//Libaray that allows to send mail from ESP32 using your gmaill account
#include <Firebase_ESP_Client.h>//For Firebase connection with ESP-32
#include <LiquidCrystal_I2C.h>

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  


//-------tokens-------
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"


//------definend Globals for Firebase Authentication------
#define USER_EMAIL "rajaehsanriaz@gmail.com"
#define USER_PASSWORD "Thelionking"
#define API_KEY "AIzaSyCMFMYfeKKE5xifcep3wLcGE4xoh4GyUWc"
// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://esp32-data-94dd1-default-rtdb.asia-southeast1.firebasedatabase.app/" 

//-----------wifi credentials-----------
#define Wifi_net "AndroidAP"
#define Wifi_pass "rajaamjad"
#define Timeout 20000


// --------The gmail credentials--------- 
#define SMTP_HOST "smtp.gmail.com"// The smtp host name e.g. smtp.gmail.com for GMail or smtp.office365.com for Outlook or smtp.mail.yahoo.com
#define SMTP_PORT 465// 465 or esp_mail_smtp_port_465
#define AUTHOR_EMAIL "rajaehsanriaz@gmail.com"
#define AUTHOR_PASSWORD "ThelionkinG.1"

//----Objects------
FirebaseData fbdo;//Define Firebase Data object
FirebaseAuth auth;//Firbase authorization ibj
FirebaseConfig config;//Firebase configure object
SMTPSession smtp;/* The SMTP Session object used for Email sending */

//--------variables-------
//defining different varaibles to be used in the project
String date_str;
String time_str;
int ir = 19;
int remaining_days=30;
String month1="";
String day1="";
String year1="";
String alrm="";//global string variable to store alarm that is to store at firebase

//--------array to store alarm--------
String alarm1[3] = {"00:00:00","00:00:00","00:00:00"};
String alarm2[3]  = {"00:00:00","00:00:00","00:00:00"};

//---------RTC credentials==========
const char *ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 18000;   // GMT offset (seconds)
const int   daylightOffset_sec = 0; // daylight offset (seconds)
//variable to make sure 1 day is decermented in 24 hours
bool flag=false;
AsyncWebServer server(80);//Defining port
 
//----Functions-----
/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);
//Function to send mail
void sendMail();
//functio nto conect ESp32 with wifi
void connect_wifi();
//function to return current local time
String printLocalTime();
//function to return current data day and year
String printdate();
//function to check if current local time mathes the alarm time
String checktime();
//function to send data to firebase
void firebase_trigger(int a,bool status,String stats);
//function for alarm
void alarm(int a);
//main setup function
void setup()
{
  Serial.begin(115200);
  pinMode(18, OUTPUT);  
  pinMode(15, OUTPUT);
  pinMode(ir, INPUT);
  // initialize LCD
  lcd.begin();
  // turn on LCD backlight                      
  lcd.backlight();
  // set cursor to first column, first row
  lcd.setCursor(1,1);

  connect_wifi();
  // init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  //firebase
  // Assign the api key 
  config.api_key = API_KEY;
  // Assign the RTDB URL 
  config.database_url = DATABASE_URL;
  auth.user.email=USER_EMAIL;
  auth.user.password=USER_PASSWORD;
  
  fbdo.setResponseSize(4096);
  Firebase.reconnectWiFi(true);
  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
   
   //routes for Web server
  server.on("/alarm", HTTP_GET, [](AsyncWebServerRequest *request){
 request->send(200, "text/html", "<h1>-------  Setting Alarm  ------</h1><form action='alarm' method='post'><label for='alarm1'>Select a time for Alarm1:</label>  <input type='time' id='alarm1' name='alarm1'><br><br><br>  <label for='alarm2'>Select a time for Alarm2:</label>  <input type='time' id='alarm2' name='alarm2'><br><br>  <input type='submit'></form>");
  });
   server.on("/alarm", HTTP_POST, [](AsyncWebServerRequest * request){
    if(request->hasArg("alarm1" )&& request->hasArg("alarm2" )){
        alarm1[0] = request->arg("alarm1")+":00" ;
        alarm1[1] = request->arg("alarm1")+":01" ;
        alarm1[2] = request->arg("alarm1")+":02" ;
        alarm2[0] = request->arg("alarm2")+":00";
        alarm2[1] = request->arg("alarm2")+":01";
        alarm2[2] = request->arg("alarm2")+":02";
        Serial.println("Alarm1 set at:"+ alarm1[0]);
        Serial.println("Alarm2 set at:"+ alarm2[0]);
    } 
    request->send(200);
 });

  server.begin();
}

//main loop function
void loop()
{
  //if wifi disconnected during functionning reconnect it again
  if ((WiFi.status() != WL_CONNECTED)) 
  {
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    if ((WiFi.status() == WL_CONNECTED))
    {
      Serial.print("Camera Ready! Use 'http://");
      Serial.print(WiFi.localIP());
      Serial.println("' to connect");
    }
    delay(3000);
  }
  delay(1000);
  lcd.clear();
  // set cursor to first column, second row
  lcd.print(printLocalTime());
//  Serial.println();
  if (checktime()=="alarm1")
  {
    alarm(1);
  }
  else if(checktime()=="alarm2")
  {
    alarm(2);
  }
  else if ((digitalRead(ir)==HIGH))
  {
    digitalWrite(15,HIGH);
    delay(500);
    digitalWrite(15,LOW);
    lcd.clear();
    lcd.print("Not now");
  }

reconnectwifi();
}

String printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return "Time Error";
  }
  char output[80];
  strftime(output, 80, "%H:%M:%S", &timeinfo);
  time_str = String(output);
  return String(output);
}


String printdate()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain date");
//    ESP.restart();
    return "Date Error";
  }
  char output[80];
  char month[10];
  char day[10];
  char year[10];
  
  strftime(output, 80, "%B %d %Y", &timeinfo);
  strftime(month, 10, "%B", &timeinfo);
  strftime(day, 10, "%d", &timeinfo);
  strftime(year, 10, "%Y", &timeinfo);
  month1 = String(month);
  day1 = String(day);
  int day11=day1.toInt();
  day11--;
  day1 =  String(day11);
  year1= String(year);
  date_str = String(output);
  return String(output);
}

String checktime()
{
  if (printLocalTime() == alarm1[0] || printLocalTime() == alarm1[1] || printLocalTime() == alarm1[2])
    {
    return "alarm1";
    }
  else if(printLocalTime() == alarm2[0] || printLocalTime() == alarm2[1] || printLocalTime() == alarm2[2])
    {
    return "alarm2";
    }
    else
    return "";
}

void firebase_trigger(int a,bool status,String stats="")
{
 if (Firebase.ready()){
    // Writing data on the database path 
    if(a==1)//if alarm 1
    {
      Firebase.RTDB.setString(&fbdo, "ESP-data/"+printdate()+"/alarm1",alarm1[0] );
      if(stats=="missed" && status==true)
      {
        Firebase.RTDB.setString(&fbdo, "ESP-data/"+printdate()+"/status1","missed" );
        Firebase.RTDB.setString(&fbdo, "ESP-data/"+printdate()+"/time1"," ");
      }
      else 
      {
        Firebase.RTDB.setString(&fbdo, "ESP-data/"+printdate()+"/status1","taken" );
        if(status==true && stats=="")
          Firebase.RTDB.setString(&fbdo, "ESP-data/"+printdate()+"/time1","late");
        else if(status==false && stats=="")
          Firebase.RTDB.setString(&fbdo, "ESP-data/"+printdate()+"/time1","ontime");
      }
    }
    
    else if (a==2)//if alarm 2
    {
      Firebase.RTDB.setString(&fbdo, "ESP-data/"+printdate()+"/alarm2",alarm2[0] );
      if(stats=="missed" && status==true)
      {
        Firebase.RTDB.setString(&fbdo, "ESP-data/"+printdate()+"/status2","missed" );
        Firebase.RTDB.setString(&fbdo, "ESP-data/"+printdate()+"/time2"," ");
      }
      else 
      {
        Firebase.RTDB.setString(&fbdo, "ESP-data/"+printdate()+"/status2","taken" );
        if(status==true && stats=="")
          Firebase.RTDB.setString(&fbdo, "ESP-data/"+printdate()+"/time2","late");
        else if(status==false && stats=="")
          Firebase.RTDB.setString(&fbdo, "ESP-data/"+printdate()+"/time2","ontime");
      }
    
    }
    Serial.println(month1+" "+day1+" "+year1);
     if(flag==true)
     {
      flag=false;
     }
     else if(flag==false)
     {
      Firebase.RTDB.getInt(&fbdo, "ESP-data/"+month1+" "+day1+" "+year1+"/remainingdays");
      remaining_days = fbdo.intData();
      remaining_days--;
      Firebase.RTDB.setInt(&fbdo, "ESP-data/"+printdate()+"/remainingdays",remaining_days);
      flag=true;
      }
 }
}
void alarm(int a)
{
  sendMail("Its time for you to take your Pills");
  int time=millis();
  while (digitalRead(ir) == LOW && millis()-time<= 20000)
  {
    lcd.print("Its Time");
    digitalWrite(18, HIGH);
    delay(1000);

    digitalWrite(18, LOW);
    delay(1000);
    Serial.print(".");
  }
  if(digitalRead(ir)!=LOW)
  {
    digitalWrite(18, LOW);
    if(millis()-time<10000)
    {
      firebase_trigger(a,false);
      lcd.clear();
      sendMail("Pill Taken on Time");
      lcd.print("Taken early");
    }
    else 
    {
      firebase_trigger(a,true);
      sendMail("Pill Taken but you are late.");
      lcd.clear();
      lcd.print("Taken late");
    }
  }
  else
  {
    firebase_trigger(a,true,"missed");
    sendMail("Pill Missed now wait for next alarm");
    lcd.clear();
    lcd.print("You're late:missed");  
  }
}
void connect_wifi()
{
  Serial.print("Connecting...");
  WiFi.mode(WIFI_AP);
  WiFi.begin(Wifi_net, Wifi_pass);
  // timeout to stop checking for wifi connection after 20 seconds
  unsigned long startattmpt = millis();
  // to make sure were still within our timeout we will subtract startattempttime from current uptime and makesure its less then timeout
  while (WiFi.status() != WL_CONNECTED && millis() - startattmpt < Timeout)
  {
    Serial.print(".");
    delay(100);
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Failed");
//    ESP.restart();
  }
  else
  {
    Serial.print("Connected");
    Serial.println(WiFi.localIP());
  }
}
void sendMail(String Msg)
{

  // Enable the debug via Serial port  1 for basic level debugging
  smtp.debug(1);
  // Set the callback function to get the sending results 
  smtp.callback(smtpCallback);
  // Declare the session config data 
  ESP_Mail_Session session;
  // Set the session config 
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;

  
  // Declare the message class 
  SMTP_Message message;
  // Set the message headers 
  message.sender.name = "Ehsan";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "Pills Reminder";
  message.addRecipient("Ehsan", "rajputjanjua1234@gmail.com");
  //store message that is needed to be send
  String textMsg = Msg;
  message.text.content = textMsg.c_str();
  
  message.text.charSet = "us-ascii";
  // The content transfer encoding 
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  // The message priority
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  // The Delivery Status Notifications 
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;
  // Set the custom message header //
  message.addHeader(F("Message-ID: <test.send@gmail.com>"));
  
  // Connect to server with the session config //
  if (!smtp.connect(&session))
    return;
  // Start sending Email and close the session 
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
}

void smtpCallback(SMTP_Status status)
{
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success())
  {

    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failed: %d\n", status.failedCount());
    Serial.println("----------------\n");

    Serial.println("----------------\n");

    // You need to clear sending result as the memory usage will grow up.
    smtp.sendingResult.clear();
  }
}

//reconnect function
void reconnectwifi()
{ 
    unsigned long currentMillisforwifi = millis();
    // if WiFi is down, try reconnecting
    if ((WiFi.status() != WL_CONNECTED) && (currentMillisforwifi - previousMillisforwifi >=interval)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    while(WiFi.disconnect()){
      Serial.println("Reconnecting...");
      ESP.restart();
      delay(1000); 
    }
    previousMillisforwifi = currentMillisforwifi;
    }
}