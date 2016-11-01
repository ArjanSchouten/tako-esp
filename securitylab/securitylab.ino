#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define FASTLED_ESP8266_D1_PIN_ORDER

#include "http.h"
#include "TimerObject.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266httpUpdate.h>
#include <QueueArray.h>
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
#define NUM_LEDS    10
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

// Fade in fade out settings
int newMessagefadeAmount = 5;
int newMessageBrightness = 10;

// The brightness fade amount for a new message
int showMessagefadeAmount = 10;
// The current brightness of the new message color
int showMessageBrightness = 10;
// The amount of times that the message already faded 
int currentShowMessageAmount = 0;
// The amount of times that a message should fade in and fade out
const int FADE_MESSAGE_TIMES = 5;

bool isRecording = false;
bool isShowingMessage = false;

// make an AnalogMultiButton object, pass in the pin, total and values array
AnalogMultiButton buttons(BUTTONS_PIN, BUTTONS_TOTAL, BUTTONS_VALUES);

const String version = "1";
const char *host = "arjan-schouten.nl";
const int port = 4443;

const long pingInterval = 5000;

//Create a http handler for connection with the server
Http http = Http(host, port);

//Setup the server with the
ESP8266WebServer server(80);

const char *deviceId = "test";

TimerObject *pingServerTimer = new TimerObject(10000, &pingServer);
TimerObject *newMessageNotificationTimer = new TimerObject(100, &showNewMessageNotification);
TimerObject *showMessageTimer = new TimerObject(200, &showMessage);

QueueArray<String> messageQueue;

void setup() {
  // setup a delay for calibrating the wifi module
  delay(1000);
  Serial.begin(115200);
#ifdef DEBUG

  //baudrate setting
  Serial.println();
  Serial.print("Current firmware version: ");
  Serial.println(version);

  Serial.print("Is WiFi configured: ");
  Serial.println(isConfigured() ? "Yes" : "No");
#endif

  // Enable the WiFi module
  WiFi.mode(WIFI_STA);
  WiFi.begin();
  WiFi.setAutoReconnect(true);

  if (!isConfigured()) {
    scanWiFiNetworks();
  } else {
    pingServerTimer->Start();
  }

  // LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  // update the AnalogMultiButton object every loop
  updateTimers();

  if (!messageQueue.isEmpty() && !isShowingMessage && !isRecording && !newMessageNotificationTimer->isEnabled()) {
    newMessageNotificationTimer->Start();
  }

  //  Debugging buttons
  //  Serial.println(analogRead(A0));

  if (buttons.onPress(BUTTON_RECIEVE) && !messageQueue.isEmpty())
  {
    isShowingMessage = true;
    //Make sure that the message notifications's wont show up during the show of the kiss
    newMessageNotificationTimer->Stop();
    showMessageTimer->Start();
  }

  // check if BUTTON_SEND has just been pressed
  if (buttons.isPressed(BUTTON_SEND))
  {
    isRecording = true;
    Serial.println("Recording Message");
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Blue;
    }
    FastLED.show();
  }

  if (buttons.onRelease(BUTTON_SEND))
  {
    isRecording = false;
    Serial.println("Message is Recorded, Send message");
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Green;
    }
    FastLED.show();

    http.sendMessage(deviceId, "Hoi!!!");
  }

#ifdef DEBUG
  Serial.print("Free memory:");
  Serial.println(ESP.getFreeHeap());
#endif
}

void resetLeds() {
  for (int k = 0; k < NUM_LEDS; k++) {
    leds[k] = CRGB::Black;
    FastLED.show();
  }
}

void showNewMessageNotification() {
  Serial.println("Going to show message notification");

  //For now show one color. Fading effects may be added in the future.
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(197, 210, 229);
    leds[i].fadeLightBy(newMessageBrightness);
  }
  newMessageBrightness = newMessageBrightness + newMessagefadeAmount;
  if (newMessageBrightness <= 0 || newMessageBrightness >= 80)
    newMessagefadeAmount = -newMessagefadeAmount;
  FastLED.show();
}

void showMessage() {
  Serial.println("Show message...");
  //Take the first message from the queue without removing
  String message = messageQueue.front();

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(216, 71, 52);
    leds[i].fadeLightBy(showMessageBrightness);
  }
  FastLED.show();
  showMessageBrightness = showMessageBrightness + showMessagefadeAmount;
  if (showMessageBrightness >= 90) {
    currentShowMessageAmount ++;
    showMessageBrightness = 0;
    if (currentShowMessageAmount >= FADE_MESSAGE_TIMES) {
      currentShowMessageAmount = 0;
      isShowingMessage = false;
      messageQueue.pop();
      showMessageTimer->Stop();
      resetLeds();
    }
  }
}

void pingServer() {
  Serial.println("Sending a ping request...");
  Http::PingResult result;
  http.pingServer(deviceId, version, &result);

  if (result.message.length() > 0 && !messageQueue.isFull()) {
    Serial.println("Recieved message..." + result.message);
    messageQueue.enqueue(result.message);
  }

  if (result.update) {
    t_httpUpdate_return ret = ESPhttpUpdate.update(host, 8080, "/update/" + String(result.newversion));
    Serial.println(ESPhttpUpdate.getLastErrorString());
  }
}

void updateTimers() {
  buttons.update();
  pingServerTimer->Update();
  newMessageNotificationTimer->Update();
  showMessageTimer->Update();
  if (!isConfigured()) {
    processWiFi();
    server.handleClient();
  }
}

