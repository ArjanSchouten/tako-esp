#include<WiFiClientSecure.h>
#include<Arduino.h>

class Http {
  public:
    WiFiClientSecure client;

    struct PingResult {
      bool updateAvailable;
      String messages[];
    };

    Http(const char *host, unsigned int port) {
      this->host = host;
      this->port = port;
    }

    PingResult pingServer(String deviceId, String version) {
      unsigned long start = millis();
      PingResult result;
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

        if (client.available()) {
          String response = client.readString();
          int bodypos =  response.indexOf("\r\n\r\n") + 4;
          String body = response.substring(bodypos);
          Serial.println("Body: " + body);
        }
      }
#ifdef DEBUG
      else {
        Serial.println("Certificate fingerprint did not match");
      }

      Serial.println("Request took: " + millis() - start);
#endif

      return result;
    }

  protected:
    const char *host;
    unsigned int port;
    const char *fp = "DA:F1:6F:97:A1:36:AA:E9:D2:B9:6B:85:11:91:C2:9F:A9:3D:71:9E";
};


