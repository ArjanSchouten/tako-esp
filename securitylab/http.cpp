#include<WiFiClientSecure.h>
#include<Arduino.h>
#include <ArduinoJson.h>

class Http {
  public:
    WiFiClientSecure client;

    struct PingResult {
      bool update = false;
      char* message;
      char* newversion;
    };

    Http(const char *host, unsigned int port) {
      this->host = host;
      this->port = port;
    }

    bool pingServer(String deviceId, String version, struct PingResult* result) {
      if (!client.connect(host, port)) {
        Serial.println("connection failed");
        return false;
      }

      if (client.verify(fp, host)) {
        Serial.println("Certificate fingerprint matches");
        client.println("GET /ping HTTP/1.1");
        client.println("Firmware-Version: " + version);
        client.println("Device-ID: " + deviceId);
        client.println("Host: " + String(host) + ":" + port);
        client.print("Connection: close");
        client.print("\n\n");

        while (client.connected() && !client.available()) delay(10);

        if (client.available()) {
          String r = client.readString();
          int bodypos = r.indexOf("\r\n\r\n") + 4;
          String b = r.substring(bodypos);

          if (b.length()) {
            StaticJsonBuffer<256> jsonBuffer;
            JsonObject& root = jsonBuffer.parseObject(b);
            
            if (root.success()) {
                if (root.containsKey("message"))
                strcpy(result->message, root["message"]);
              if (root.containsKey("newversion"))
                strcpy(result->newversion, root["newversion"]);
              result->update = root["update"];
            } else {
              return false;
            }
          }
        }
      }

      return true;
    }

    bool sendMessage(String deviceId, String message) {
      if (!client.connect(host, port)) {
        Serial.println("connection failed");
        return false;
      }

      if (client.verify(fp, host)) {
        String body = "{\"message\": \"" + message + "\"}\n";
        Serial.println("Certificate fingerprint matches");
        client.print("POST /send HTTP/1.1\n");
        client.print("Device-ID: " + deviceId + "\n");
        client.print("Content-Type: application/json\n");
        client.print("Content-Length: " + String(body.length()) + "\n");
        client.println("Host: " + String(host) + ":" + port);
        client.print("Connection: close");
        client.print("\n\n");
        client.print(body);

        while (client.connected() && !client.available()) delay(10);
      }

      return true;
    }

  protected:
    const char *host;
    unsigned int port;
    const char *fp = "DA:F1:6F:97:A1:36:AA:E9:D2:B9:6B:85:11:91:C2:9F:A9:3D:71:9E";
};


