/*The ESPAsyncWebServer library to create our web server easily.
 With this library, we will set an asynchronous HTTP server.
  AsyncTCP is another library that we will be incorporating as 
  it a dependency for the ESPAsyncWebServer library. */
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "WiFi.h"
 
 //Wifi credentials
const char* ssid = "AndroidAP";
const char* password =  "rajaamjad";
 
// Creating a AsyncWebServer object 
AsyncWebServer server(80);
 
 //setup function
void setup(){
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println(WiFi.localIP());
 // Route for root /post web page(Get request)
  server.on("/post", HTTP_GET, [](AsyncWebServerRequest *request){
      //sending html as a get request for /post
    request->send(200, "text/html", "<form action='/post' method='post'>  <label for='fname'>First name:</label><br>  <input type='text' id='fname' name='fname' value='John'><br>  <label for='lname'>Last name:</label><br>  <input type='text' id='lname' name='lname' value='Doe'><br><br>  <input type='submit' value='Submit'></form>"
);
  });

 // Route for root /post web page(Postss request)
 server.on("/post", HTTP_POST, [](AsyncWebServerRequest * request){
     //getting values of arguments in post req body and copying it in our own variables
    if(request->hasArg("fname" )&& request->hasArg("lname" )){
        String arg = request->arg("fname");
        String arg1 = request->arg("lname");
        Serial.print("The number is: ");
        Serial.println(arg);
        Serial.println(arg1);
    } 
    request->send(200);
 });

  server.begin();
}
 
void loop(){}