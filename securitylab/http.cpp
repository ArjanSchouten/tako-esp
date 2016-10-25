#include <WiFiClientSecure.h>

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

    PingResult pingServer() {
      PingResult result;
      Serial.println("Connecting to " + String(host) + "...");
      if (!client.connect(host, port)) {
        Serial.println("connection failed");
        return result;
      }

      Serial.println("Verifying certificate " + String(fp));
      if (client.verify(fp, host)) {
        Serial.println("Certificate fingerprint matches");
        client.print("GET /ping HTTP/1.1\n");
        client.print(String("Host: ") + host + ":" + port + "\n");
        client.print("Connection: close\n");
        client.print("\n\n");

        Serial.println(client.readStringUntil('\n'));
      }
      else
        Serial.println("Certificate fingerprint did not match");
      return result;
    }

    protected:
      const char *host;
      unsigned int port;
      const char *fp = "DA:F1:6F:97:A1:36:AA:E9:D2:B9:6B:85:11:91:C2:9F:A9:3D:71:9E";
};


