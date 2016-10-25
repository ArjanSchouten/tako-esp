s#include "storage.cpp"
#include "http.cpp"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>

#define DEBUG=1
#define STORAGEDEBUG=1

const String version = "1.0.0";
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

  if (sizeof wifiSettings.ssid <= 0) {
    // Wifi_accespoint.ino setup a http server on the esp8266
    SetupApHttp();
  }
  else {
    connectWifi();
  }
}

void connectWifi() {
  WiFi.begin(wifiSettings.ssid, wifiSettings.password);
  WiFi.setAutoReconnect(true);
}

void loop() {
  //Wifi accespoint code
  server.handleClient();

  if (WiFi.status() == WL_CONNECTED) {
    Http::PingResult* result = http.pingServer(deviceId.deviceId, version);
    Serial.println(result->message);
    Serial.print("Must update?");
    Serial.println(result->updateAvailable ? "Yes": "No");
    free(result);
  } else {
    connectWifi();
  }

  delay(5000);
  processWiFi();
}

