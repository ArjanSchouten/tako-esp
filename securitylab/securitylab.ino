#include "http.cpp"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266httpUpdate.h>

const String version = "1.0.1";
const char *host = "arjan-schouten.nl";
const int port = 4443;

#define DEBUG

//Create a http handler for connection with the server
Http http = Http(host, port);

//Setup the server with the 
ESP8266WebServer server(80);

// Pin definitions
const int RED_LED_PIN = 5;
const int GREEN_LED_PIN = 4;
const int YELLOW_LED_PIN = 3;
const int SEND_BUTTON_PIN = 2;
const int RECIEVE_BUTTON_PIN = 1;
const int RESET_BUTTON_PIN = 0;

const char *ssid = "ESP_AP";
const char *password = "testtest";
const char *deviceId = "test";

void setup() {
  // setup a delay for calibrating the wifi module
  delay(1000);

#ifdef DEBUG
  Serial.begin(115200);
  //baudrate setting
  Serial.println();
  Serial.println(version);
#endif

  WiFi.begin(ssid, password);
  WiFi.setAutoReconnect(true);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
#ifdef DEBUG
    Serial.print(".");
#endif
  }
}

void loop() {
  //Wifi accespoint code
  server.handleClient();

  Http::PingResult result = http.pingServer(deviceId, version);
  Serial.println(result.message);
  
  http.sendMessage(deviceId, "Hoi!!!");

  if (result.update) {
    t_httpUpdate_return ret = ESPhttpUpdate.update(host, 8080, "/update/1.0.1");
    Serial.println(ESPhttpUpdate.getLastErrorString());
  }

  processWiFi();
#ifdef DEBUG
  Serial.print("Free memory:");
  Serial.println(ESP.getFreeHeap());
#endif
}

