//----------Included Libraries---------
#include <Arduino.h>
#include <WiFi.h>
#include "time.h"
//Libaray that allows to send mail from ESP32 using your gmaill account
#include <ESP_Mail_Client.h>
//For Firebase connection with ESP-32
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

//------definend Globals------

#define USER_EMAIL "rajaehsanriaz@gmail.com"
#define USER_PASSWORD "Thelionking"
#define API_KEY "AIzaSyCMFMYfeKKE5xifcep3wLcGE4xoh4GyUWc"
// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://esp32-data-94dd1-default-rtdb.asia-southeast1.firebasedatabase.app/" 
//wifi credentials
#define Wifi_net "AndroidAP"
#define Wifi_pass "rajaamjad"
#define Timeout 20000
// The smtp host name e.g. smtp.gmail.com for GMail or smtp.office365.com for Outlook or smtp.mail.yahoo.com
#define SMTP_HOST "smtp.gmail.com"
// 465 or esp_mail_smtp_port_465
#define SMTP_PORT 465
// The sign in credentials 
#define AUTHOR_EMAIL "rajaehsanriaz@gmail.com"
#define AUTHOR_PASSWORD "******"

//----Objects------

//Define Firebase Data object
FirebaseData fbdo;
//Firbase authorization ibj
FirebaseAuth auth;
//Firebase configure object
FirebaseConfig config;
//string to store alarm
/* The SMTP Session object used for Email sending */
SMTPSession smtp;

//--------variables-------

//defining different varaibles to be used in the project
String date_str;
String time_str;
int ir = 19;
string alrm="";
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

//----Functions-----

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);
//Function to send mail
void sendMail()
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
  String textMsg = "Do it";
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
    return;]
  // Start sending Email and close the session 
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
}
//functio nto conect ESp32 with wifi
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
    // take any action
  }
  else
  {
    Serial.print("Connected");
    Serial.println(WiFi.localIP());
  }
}
//function to return current local time
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

//function to return current data day and year
String printdate()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain date");
    return "Date Error";
  }
  char output[80];
  strftime(output, 80, "%B %d %Y", &timeinfo);
  date_str = String(output);
  return String(output);
}
//main setup function
void setup()
{
  Serial.begin(115200);
  pinMode(18, OUTPUT);
  pinMode(ir, INPUT);
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
}

//function to check if current local time mathes the alarm time
bool checktime()
{
  if (printLocalTime() == "19:37:27" || printLocalTime() == "19:37:27" || printLocalTime() == "19:37:27")
    alrm=printLocalTime();
    return true;
  else
    return false;
}
void firebase_trigger(bool status)//sattus variable to tell whether pill was taken on time or not
{
 if (Firebase.ready()){
    // Writing data on the on real time database path
    Firebase.RTDB.setInt(&fbdo, "ESP-data/"+printdate()+"/alarm1",alrm );
    Firebase.RTDB.setInt(&fbdo, "ESP-data/"+printdate()+"/status","taken" );
    if(status==true)
    Firebase.RTDB.setInt(&fbdo, "ESP-data/"+printdate()+"/status1","late");
    else
    Firebase.RTDB.setInt(&fbdo, "ESP-data/"+printdate()+"/status1","ontime");

}
//function for alarm
void alarm()
{
  int time=millis();//storing time to keep track of time
  sendMail();//sending mail
  while (digitalRead(ir) == LOW)
  {
    digitalWrite(18, HIGH);
    delay(1000);

    digitalWrite(18, LOW);
    delay(1000);
  }
  digitalWrite(18, LOW);
  if(millis()-time>10000)
  firebase_trigger(false);//late
  else 
  firebase_trigger(true);//ontime
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
//main loop function
void loop()
{
  delay(1000);
  Serial.println(printLocalTime());
  if (checktime())
  {
    alarm();
  }
}