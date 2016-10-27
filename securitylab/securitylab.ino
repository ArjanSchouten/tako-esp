#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define FASTLED_ESP8266_D1_PIN_ORDER

#include "http.cpp"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266httpUpdate.h>
/* include button library */
#include <AnalogMultiButton.h>
/* include led library */
#include <FastLED.h>

FASTLED_USING_NAMESPACE

// Remove DEBUG for none debug mode
#define DEBUG

//Define property's for led strip
#define DATA_PIN    5
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    6
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

// Buttons Pin
const int BUTTONS_PIN = A0;
// buttons arary
const int BUTTONS_TOTAL = 2;
const int BUTTONS_VALUES[BUTTONS_TOTAL] = {313, 475};
// button names
const int BUTTON_RECIEVE = 0;
const int BUTTON_SEND = 1;

// make an AnalogMultiButton object, pass in the pin, total and values array
AnalogMultiButton buttons(BUTTONS_PIN, BUTTONS_TOTAL, BUTTONS_VALUES);

const String version = "1";
const char *host = "arjan-schouten.nl";
const int port = 4443;

const long pingInterval = 5000;

//Create a http handler for connection with the server
Http http = Http(host, port);

long previousPingMillis;

//Setup the server with the
ESP8266WebServer server(80);

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
  Serial.print("Current firmware version: ");
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

  // LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  previousPingMillis = millis();
}

void loop() {
  // update the AnalogMultiButton object every loop
  buttons.update();

  // Ping the server on a specified interval
  if (millis() > previousPingMillis + pingInterval) {
    Serial.println("Sending a ping request...");
    Http::PingResult result;
    http.pingServer(deviceId, version, &result);

    if (result.update) {
      t_httpUpdate_return ret = ESPhttpUpdate.update(host, 8080, "/update/" + String(result.newversion));
      Serial.println(ESPhttpUpdate.getLastErrorString());
    }

    previousPingMillis = millis();
  }

  //  Debugging buttons
  //  Serial.println(analogRead(A0));

  if (buttons.onPress(BUTTON_RECIEVE))
  {
    Serial.println("Receive message");

    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Orange;
      FastLED.show();
      delay(1000);
    }
    delay(2000);
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Red;
      FastLED.show();
    }
    delay(3000);

    for (int k = 0; k < NUM_LEDS; k++) {
      leds[k] = CRGB::Black;
      FastLED.show();
    }
  }

  // check if BUTTON_SEND has just been pressed
  if (buttons.isPressed(BUTTON_SEND))
  {
    Serial.println("Recording Message");
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Blue;
      FastLED.show();
    }
  }

  if (buttons.onRelease(BUTTON_SEND))
  {
    Serial.println("Message is Recorded, Send message");
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Green;
      FastLED.show();
    }

    http.sendMessage(deviceId, "Hoi!!!");

    for (int k = 0; k < NUM_LEDS; k++) {
      leds[k] = CRGB::Black;
      FastLED.show();
    }
  }

  processWiFi();
#ifdef DEBUG
  Serial.print("Free memory:");
  Serial.println(ESP.getFreeHeap());
#endif

  delay(100);
}
