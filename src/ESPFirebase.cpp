#include <Arduino.h>
#include "WiFi.h"
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "AndroidAP"
#define WIFI_PASSWORD "rajaamjad"

//user auth params


// Insert Firebase project API Key
#define USER_EMAIL "rajaehsanriaz@gmail.com"
#define USER_PASSWORD "Th*********"
#define API_KEY "AIzaSyCMFMYfeKKE5xifcep3wLcGE4xoh4GyUWc"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://esp32-data-94dd1-default-rtdb.asia-southeast1.firebasedatabase.app/" 

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;



void setup(){
  Serial.begin(115200);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

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

void loop(){
  if (Firebase.ready()){
    // Write an Int number on the database path CAO/B
    if (Firebase.RTDB.setInt(&fbdo, "CAO/B", 5)){
      Serial.println("PASSED");
    }
    else {
        Serial.println("FAILED");
    }
    
  }
}