#include<WiFiClientSecure.h>
#include<Arduino.h>
#include <ArduinoJson.h>

class Http {
  public:
    WiFiClientSecure client;

    typedef struct PingResult {
      bool updateAvailable;
      const char* message;
    } PingResult;

    Http(const char *host, unsigned int port) {
      this->host = host;
      this->port = port;
    }

    PingResult* pingServer(String deviceId, String version) {
      unsigned long start = millis();
      PingResult *result = new PingResult();
      if (!client.connect(host, port)) {
        Serial.println("connection failed");
        return result;
      }

      if (client.verify(fp, host)) {
        Serial.println("Certificate fingerprint matches");
        client.print("GET /ping HTTP/1.1\n");
        client.print("Firmware-Version: " + version + "\n");
        client.print("Device-ID: " + deviceId + "\n");
        client.print("Host: " + String(host) + ":" + port);
        client.print("\n\n");

        while (client.connected() && !client.available()) delay(10);

        if (client.available()) {
          String response = client.readString();
          int bodypos =  response.indexOf("\r\n\r\n") + 4;
          String body = response.substring(bodypos);

          if (body.length()) {
            StaticJsonBuffer<256> jsonBuffer;
            JsonObject& root = jsonBuffer.parseObject(body);

            if (root.success()) {
              result->message = root["message"].asString();
              result->updateAvailable = root["update"];
            }
          }
        }
      }
      else {
        Serial.println("Certificate fingerprint did not match");
      }

      Serial.print("Request took: ");
      Serial.println(millis() - start);

      return result;
    }

  protected:
    const char *host;
    unsigned int port;
    const char *fp = "DA:F1:6F:97:A1:36:AA:E9:D2:B9:6B:85:11:91:C2:9F:A9:3D:71:9E";
};


