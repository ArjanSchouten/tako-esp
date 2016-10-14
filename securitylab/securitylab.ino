#include "storage.cpp"
/* include wifi libraries */
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define DEBUG=1
#define STORAGEDEBUG=1

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

#ifdef DEBUG
  deviceIdStorage.val.deviceId = "This is a testThis is a test";
  storage.save(deviceIdStorage);
#endif

  deviceId = storage.read<Storage::DeviceId>(deviceIdStorage);
#ifdef DEBUG
  Serial.println(deviceId.deviceId);
#endif

  // Wifi_accespoint.ino setup a http server on the esp8266  
  SetupApHttp();
}

void loop() {
  //Wifi accespoint code 
  server.handleClient();

  processWiFi();
}

