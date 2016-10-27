#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define FASTLED_ESP8266_D1_PIN_ORDER

#include "storage.cpp"
#include "http.cpp"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
/* include button library */
#include <AnalogMultiButton.h>
/* include led library */
#include <FastLED.h>

FASTLED_USING_NAMESPACE

// Debug on=1 debug off=0
#define DEBUG=1

//Define property's for led strip
#define DATA_PIN    5
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    6
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

const String version = "1.0.0";

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

  wifiSettings.ssid = "Stads-Lab";
  wifiSettings.password = "initialkey4iot";
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
  
  // LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  
}

void loop() {
  // update the AnalogMultiButton object every loop
  buttons.update();

//  Debugging buttons 
//  Serial.println(analogRead(A0));

  if(buttons.onPress(BUTTON_RECIEVE))
  {
     Serial.println("Receive message");
    
    for(int i=0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Orange;
      FastLED.show();
      delay(1000);
    }
    delay(2000);
    for(int i=0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Red;
      FastLED.show();
    } 
    delay(3000);
    
    for(int k=0; k < NUM_LEDS; k++) {
      leds[k] = CRGB::Black;
      FastLED.show();
    }
  }

  // check if BUTTON_SEND has just been pressed
  if(buttons.isPressed(BUTTON_SEND))
  {
    Serial.println("Recording Message");
    for(int i=0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Blue;
      FastLED.show();
    }
  }
  
  if (buttons.onRelease(BUTTON_SEND)) 
  { 
    Serial .println("Message is Recorded, Send message"); 
    for(int i=0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Green;
      FastLED.show();
    }
    
    delay(2000);
    
    for(int k=0; k < NUM_LEDS; k++) {
      leds[k] = CRGB::Black;
      FastLED.show();
    }
  }

  //Wifi accespoint code
//   server.handleClient();

//   Http::PingResult result = http.pingServer(deviceId.deviceId, version);



  delay(100);
}
