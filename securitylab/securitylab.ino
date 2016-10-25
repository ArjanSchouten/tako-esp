#include "storage.cpp"
#include "http.cpp"
/* include wifi libraries */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#define DEBUG=1

const char *host = "arjan-schouten.nl";
const int port = 4443;

Http http = Http(host, port);

Storage storage;
Storage::DeviceId deviceId;

ESP8266WebServer server(80);

// Pin definitions
const int RED_LED_PIN = 5;
const int GREEN_LED_PIN = 4;
const int YELLOW_LED_PIN = 3;
const int SEND_BUTTON_PIN = 2;
const int RECIEVE_BUTTON_PIN = 1;
const int RESET_BUTTON_PIN = 0;

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
  Storage::DeviceIdStorage deviceIdStorage;
  Storage::WifiStorage wifiStorage;


#ifdef DEBUG
  deviceIdStorage.val.deviceId = "This is a testThis is a test";
  storage.save(deviceIdStorage);

  wifiStorage.val.ssid = "ESP_AP";
  wifiStorage.val.password = "testtest";
  storage.save(wifiStorage);
#endif

  deviceId = storage.read<Storage::DeviceId>(deviceIdStorage);
#ifdef DEBUG
  Serial.println(deviceId.deviceId);
#endif

  wifiSettings = storage.read<Storage::WifiSettings>(wifiStorage);
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
  //Wifi accespoint code
  server.handleClient();

  Http::PingResult result = http.pingServer();

  delay(10000);
}

