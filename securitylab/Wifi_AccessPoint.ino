#include "ESP8266WiFi.h"
#include "build/configuration.c"

//Load the external code coming from xxd which places the html contents as a char array in a c file.
extern unsigned char html_configuration_html[];

//The different WiFi statuses we have
enum WiFiStatus {UNCONFIGURED, CONNECTED, SCANNING_NETWORK, CONNECTING, WAITING_FOR_USER};

enum WiFiStatus currentWiFiStatus = UNCONFIGURED;
int networksFound;

/* Just a little test message.  Go to http://192.168.4.1 in a web browser */
void handleRoot() {
  String html = String(reinterpret_cast<const char*>(html_configuration_html));
  for (int i = 0; i < networksFound; i ++) {
    html = html + escapeHtml(WiFi.SSID(i)) + "<br/>";
  }
  server.send(200, "text/html", html);
}

/**
   Validate the WiFi SSID following the IEEE standard
   @See http://standards.ieee.org/getieee802/download/802.11-2007.pdf

   The maximum length of a SSID is 32 characters with an unspecified characterset
*/
bool isValidSsid(String ssid) {
  //For WL_SSID_MAX_LENGTH: See https://github.com/esp8266/Arduino/blob/4897e0006b5b0123a2fa31f67b14a3fff65ce561/libraries/ESP8266WiFi/src/include/wl_definitions.h
  if (ssid.length() > WL_SSID_MAX_LENGTH)
    return false;

  return true;
}

bool isValidUsername(String username) {
  return true;
}

bool isValidPassword(String password) {
  return true;
}

void saveConfiguration() {
  String user = server.arg("user");
  String password = server.arg("passUser");
  String ssid = server.arg("ssidUser");

  if (isValidSsid(ssid) && isValidUsername(user) && isValidPassword(password)) {
    server.send(200, "text/html", "<html><body>Saved...</body></html>");
  }
}

void setupServer(int networksFound) {
  Serial.print("Found networks: ");
  Serial.println(networksFound);
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Tako-AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/save", saveConfiguration);
  server.begin();
  Serial.println("HTTP server started");
}

void SetupApHttp() {
  const bool showHiddenNetworks = true;
  const bool scanNetworksAsync = true;
  WiFi.scanNetworks(scanNetworksAsync, showHiddenNetworks);
  currentWiFiStatus = SCANNING_NETWORK;
}

void processWiFi() {
  switch (currentWiFiStatus)
  {
    case SCANNING_NETWORK:
      {
        networksFound = WiFi.scanComplete();
        if (networksFound > 0) {
          currentWiFiStatus = WAITING_FOR_USER;
          setupServer(networksFound);
        }
      }
      break;
  }
}

String escapeHtml(String param)  {
  param.replace("+", " ");
  param.replace("%21", "!");
  param.replace("%23", "#");
  param.replace("%24", "$");
  param.replace("%26", "&");
  param.replace("%27", "'");
  param.replace("%28", "(");
  param.replace("%29", ")");
  param.replace("%2A", "*");
  param.replace("%2B", "+");
  param.replace("%2C", ",");
  param.replace("%2F", "/");
  param.replace("%3A", ":");
  param.replace("%3B", ";");
  param.replace("%3D", "=");
  param.replace("%3F", "?");
  param.replace("%40", "@");
  param.replace("%5B", "[");
  param.replace("%5D", "]");

  return param;
}

