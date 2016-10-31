#include "C:/Data-Arjan-development/securitylab/tako-esp/securitylab/http.cpp"
#include "ESP8266WiFi.h"

Http http = Http("arjan-schouten.nl", 4443);

const String version = "2";
const String deviceId = "test";

void setup() {
  Serial.begin(115200);
  Serial.println();
  WiFi.begin("ESP_AP", "testtest");

  http.pingUrl = "/ping-fuzz";
}

void loop() {
  Http::PingResult result;
  http.pingServer(deviceId, version, &result);
  Serial.println(result.message);
}
