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

void setup() {
  // setup a delay for calibrating the wifi module
  delay(1000);
  //baudrate setting 
  Serial.begin(115200);
  Serial.println();

  // Wifi_accespoint.ino setup a http server on the esp8266  
  SetupApHttp();

}

void loop() {
  //Wifi accespoint code 
  server.handleClient();
}


