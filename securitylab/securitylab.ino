/* include wifi libraries */
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

ESP8266WebServer server(80); 

// Pin definitions
const int RED_LED_PIN = 5;
const int GREEN_LED_PIN = 4;
const int YELLOW_LED_PIN = 3;
const int SEND_BUTTON_PIN = 2;
const int RECIEVE_BUTTON_PIN = 1;
const int RESET_BUTTON_PIN = 0;

const char *ssid = "Tako Setup";
const char *password = "HalloTako";

/* Just a little test message.  Go to http://192.168.4.1 in a web browser */
void handleRoot() {
  server.send(200, "text/html", "<h1>Tako is online</h1>");
}

void setup() {

  delay(1000);
  Serial.begin(115200);
  Serial.println();

  /* Wifi accespoint code */
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);
  
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Tako-AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
  /* End code wifi acces point */
   
}

void loop() { 
  /* Wifi accespint code */
  server.handleClient();
  /* End code wifi acces point*/
}
