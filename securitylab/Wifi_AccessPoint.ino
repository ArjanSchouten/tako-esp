

/* Just a little test message.  Go to http://192.168.4.1 in a web browser */
void handleRoot() {
  server.send(200, "text/html", 
  "<html><body><form name='credentials' method='get'>User Name:<input type='text' name='user'><br><br>Wifi Name<input type='text' name='ssidUser'><br><br>Password<input type='password' name='passUser'><br><br><input type='submit' value='Done'></form></body></html>");
}

void SetupApHttp() {
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Tako-AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}


