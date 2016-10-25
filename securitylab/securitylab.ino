#include "storage.cpp"
#include "http.cpp"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
/* include button library */
#include <AnalogMultiButton.h>

// Debug on=1 debug off=0
#define DEBUG=1

const String version = "1.0.0";

// Buttons Pin
const int BUTTONS_PIN = A0;
// buttons arary
const int BUTTONS_TOTAL = 3;
const int BUTTONS_VALUES[BUTTONS_TOTAL] = {5, 313, 475};
// button names
const int BUTTON_RECIEVE = 0;
const int BUTTON_SEND = 1;
const int BUTTON_RESET = 2;

// make an AnalogMultiButton object, pass in the pin, total and values array
AnalogMultiButton buttons(BUTTONS_PIN, BUTTONS_TOTAL, BUTTONS_VALUES);

const char *host = "arjan-schouten.nl";
const int port = 4443;

Http http = Http(host, port);

Storage storage;
Storage::DeviceId deviceId;

ESP8266WebServer server(80);

Storage::WifiSettings wifiSettings;
const char *ssid = "Tako Setup";
const char *password = "HalloTako";

void setup() {
  // setup a delay for calibrating the wifi module
  delay(1000);

#ifdef DEBUG
  Serial.begin(115200);
  //baudrate setting
  Serial.println();
#endif

  storage.init();

#ifdef DEBUG
  deviceId.deviceId = "test";

  wifiSettings.ssid = "ESP_AP";
  wifiSettings.password = "testtest";
#endif

  storage.read<Storage::DeviceId>(Storage::DEVICEID, &deviceId);
  storage.read<Storage::WifiSettings>(Storage::WIFI_SETTINGS_ID, &wifiSettings);

#ifdef DEBUG
  Serial.println("Device-ID: " + String(deviceId.deviceId));
  Serial.println("WiFi SSID: " + String(wifiSettings.ssid));
#endif

  Serial.println(sizeof(wifiSettings.ssid));
  if (sizeof(wifiSettings.ssid) <= 0) {
    // Wifi_accespoint.ino setup a http server on the esp8266
    SetupApHttp();
  }
  else {
    Serial.println("Connecting to Wifi network");
    WiFi.begin(wifiSettings.ssid, wifiSettings.password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  }
}

void loop() {
  // update the AnalogMultiButton object every loop
  buttons.update();

  if(buttons.isPressed(BUTTON_RECIEVE))
  {
     Serial.println("Receive message");
  } 

  // check if BUTTON_GREEN has just been pressed this update
  if(buttons.isPressed(BUTTON_SEND))
  {
    Serial.println("Message is Send");
  }

  // check if BUTTON_GREEN has just been pressed this update
  if(buttons.isPressed(BUTTON_RESET))
  {
    Serial.println("Reset Tako");
  }
  
  //Wifi accespoint code
  server.handleClient();

  Http::PingResult result = http.pingServer(deviceId.deviceId, version);

  

  delay(1000);
}

