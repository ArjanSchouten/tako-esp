#include "ESP8266WiFi.h"
#include "build/configuration.c"
#include <DNSServer.h>

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);

//Load the external code coming from xxd which places the html contents as a char array in a c file.
extern unsigned char __html_configuration_html[];

//The different WiFi statuses we have
enum WiFiStatus {IDLE, SCANNING_NETWORK, CONNECTING, WAITING_FOR_USER};

enum WiFiStatus currentWiFiStatus = IDLE;
int networksFound;

DNSServer dnsServer;

/* Just a little test message.  Go to http://192.168.4.1 in a web browser */
void handleRoot() {
  String html = String(reinterpret_cast<const char*>(__html_configuration_html));

  String ssid;
  for (int i = 0; i < networksFound; i ++) {
    ssid = ssid + escapeHtml(WiFi.SSID(i)) + "<br/>";
  }
  html.replace("{ssid}", ssid);
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
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  dnsServer.setTTL(300);
  dnsServer.start(DNS_PORT, "mytako.local", apIP);
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);

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
  if (currentWiFiStatus == IDLE) {
    const bool showHiddenNetworks = true;
    const bool scanNetworksAsync = true;
    WiFi.scanNetworks(scanNetworksAsync, showHiddenNetworks);
    currentWiFiStatus = SCANNING_NETWORK;
  }
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
    case WAITING_FOR_USER:
      {
        dnsServer.processNextRequest();
      }
      break;
  }
}










