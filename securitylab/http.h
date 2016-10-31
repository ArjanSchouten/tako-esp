#include<WiFiClientSecure.h>
#include<Arduino.h>
#include <ArduinoJson.h>

#ifndef HTTP_H
#define HTTP_H
class Http {
  public:
    WiFiClientSecure client;

    typedef struct PingResult {
      bool update = false;
      String message;
      String newversion;
    };

    Http(const char *host, unsigned int port);
    bool pingServer(String deviceId, String version, PingResult* result);
    bool sendMessage(String deviceId, String message);
    String pingUrl = "/ping";
  
  protected:
    const char *host;
    unsigned int port;
    const char *fp = "DA:F1:6F:97:A1:36:AA:E9:D2:B9:6B:85:11:91:C2:9F:A9:3D:71:9E";
};
#endif
