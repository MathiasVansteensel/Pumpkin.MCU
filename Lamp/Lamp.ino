#include "Util.cpp"

void setup() {
  // Start WiFi in access point mode
  const char* ssidAndPass = GetSsidAndPass();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssidAndPass, ssidAndPass);

  Serial.println();
  Serial.print("Access Point started with SSID: ");
  Serial.println(defaultSsid);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  // Start UDP server
  udp.begin(localUdpPort);
  Serial.print("UDP server started on port ");
  Serial.println(localUdpPort);

  // Check for UDP packets
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char packetBuffer[255];
    int len = udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    if (len > 0) {
      packetBuffer[len] = '\0';
      Serial.print("Received packet: ");
      Serial.println(packetBuffer);

      // Extract SSID and password from the packet
      String receivedData = String(packetBuffer);
      String receivedSsid = receivedData.substring(0, receivedData.indexOf(':'));
      String receivedPassword = receivedData.substring(receivedData.indexOf(':') + 1);

      // Connect to the received network
      Serial.print("Connecting to network: ");
      Serial.println(receivedSsid);
      WiFi.begin(receivedSsid.c_str(), receivedPassword.c_str());

      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }

      Serial.println();
      Serial.println("Connected to network");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    }
  }
}

void loop() {

}

void Fade(bool cancel) //TODO: reset to current static color
{
  uint16_t startTime = millis();
  for(short c = 0; c <= 255; c++)
  {
    rgbColor[G] = c;
    analogWrite(rgbLedPins[G], 255 - rgbColor[G]);
    PrintCurrentColor();
    while (!cancel && startTime + 4 > millis()) {}
  }

  startTime = millis();
  for(short c = 255; c >= 0; c--)
  {
    rgbColor[R] = c;
    analogWrite(rgbLedPins[R], 255 - rgbColor[R]);
    PrintCurrentColor();
    while (!cancel && startTime + 4 > millis()) {}
  }

  startTime = millis();
  for(short c = 0; c <= 255; c++)
  {
    rgbColor[B] = c;
    analogWrite(rgbLedPins[B], 255 - rgbColor[B]);
    PrintCurrentColor();
    while (!cancel && startTime + 4 > millis()) {}
  }

  startTime = millis();
  for(short c = 255; c >= 0; c--)
  {
    rgbColor[G] = c;
    analogWrite(rgbLedPins[G], 255 - rgbColor[G]);
    PrintCurrentColor();
    while (!cancel && startTime + 4 > millis()) {}
  }

  startTime = millis();
  for(short c = 0; c <= 255; c++)
  {
    rgbColor[R] = c;
    analogWrite(rgbLedPins[R], 255 - rgbColor[R]);
    PrintCurrentColor();
    while (!cancel && startTime + 4 > millis()) {}
  }

  startTime = millis();
  for(short c = 255; c >= 0; c--)
  {
    rgbColor[B] = c;
    analogWrite(rgbLedPins[B], 255 - rgbColor[B]);
    PrintCurrentColor();
    while (!cancel && startTime + 4 > millis()) {}
  }
}
