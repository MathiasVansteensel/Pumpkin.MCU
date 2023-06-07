void WaitNetworkCred(NetworkCredentials& credentials)
{
    //const char* ssidAndPass = GetSsidAndPass();

    ConnectToNetwork(WIFI_AP, NetworkCredentials(MODULE_TYPE, nullptr));

    while (true)
    {
        int packetSize = udp.parsePacket();
        if (packetSize)
        {
            char packetBuffer[255];
            int len = udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
            packetBuffer[len] = '\0';
            String receivedData = String(packetBuffer);
            if (len > 0 && packetBuffer[0] == (uint8_t)'#')
            {
                
                Serial.print("Received packet: ");
                Serial.println(receivedData);

                int splitIndex = receivedData.indexOf(':');
                String receivedSsid = receivedData.substring(1, splitIndex);
                String receivedPassword = receivedData.substring(splitIndex + 1);

                credentials = NetworkCredentials(receivedSsid.c_str(), receivedPassword.c_str());
                Serial.print("Received SSID: ");
                Serial.println(credentials.Ssid);
                Serial.print("Received Password: ");
                Serial.println(credentials.Password);
                ConnectToNetwork(WIFI_STA, credentials);
                return;
            }
        }
    }
}

void GetNetworkCredentials()
{
    NetworkCredentials credentials;
    uint8_t eepromVal = EEPROM.read(NetCredSetAddr);
    if (eepromVal == 0xFF) EEPROM.write(NetCredSetAddr, eepromVal = false); //only executes once over the lifetime of the mcu *normally anyway*
    if (eepromVal) EEPROM.get(NetCredAddr, credentials);
    Serial.println("no credentials found in EEPROM, waiting to receive");
    WaitNetworkCred(credentials);
    EEPROM.put(NetCredAddr, credentials);
    EEPROM.write(NetCredSetAddr, true);   
}

void ConnectToNetwork(WiFiMode_t mode, NetworkCredentials credentials)
{
    bool isAP = mode == WIFI_AP;
    WiFi.mode(mode);
    if (isAP) WiFi.softAP(credentials.Ssid, credentials.Password);
    else WiFi.begin(credentials.Ssid, credentials.Password);

    Serial.print(!isAP ? "Connecting to: ": "Starting AP: ");
    Serial.println(credentials.Ssid);
    while (!isAP && WiFi.status() != WL_CONNECTED)
    {
        delay(350);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Success!");
    Serial.print("IP address: ");
    Serial.println(isAP ? WiFi.softAPIP() : WiFi.localIP());

    udp.begin(localUdpPort);
    Serial.print("UDP server started on port ");
    Serial.println(localUdpPort);
}