#include "http.h"

Http::Http(const char *host, unsigned int port) {
  this->host = host;
  this->port = port;
}

bool Http::pingServer(String deviceId, String version, PingResult* result) {
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return false;
  }

  if (client.verify(fp, host)) {
    Serial.println("Certificate fingerprint matches");
    client.println("GET " + Http::pingUrl +" HTTP/1.1");
    client.println("Firmware-Version: " + version);
    client.println("Device-ID: " + deviceId);
    client.println("Host: " + String(host) + ":" + port);
    client.print("Connection: close");
    client.print("\n\n");

    while (client.connected() && !client.available()) delay(10);

    if (client.available()) {
      String r = client.readString();
      Serial.println(r);
      int bodypos = r.indexOf("\r\n\r\n") + 4;
      String b = r.substring(bodypos);

      if (b.length()) {
        StaticJsonBuffer<256> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(b);

        if (root.success()) {
          if (root.containsKey("message"))
            result->message = root["message"].asString();
          if (root.containsKey("newversion"))
            result->newversion = root["newversion"].asString();
          result->update = root["update"];
        } else {
          return false;
        }
      }
    }
  }

  return true;
}

bool Http::sendMessage(String deviceId, String message) {
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

