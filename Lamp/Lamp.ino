#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include "Util.cpp"
#include "NetworkCredentials.h"

#define UDP_TX_PACKET_MAX_SIZE 8192
#define R 0
#define G 1
#define B 2
#define SAMPLEDELAY_MINUTE 0.5
#define NetCredSetAddr 0x0
#define NetCredAddr 0x69

const uint16_t localUdpPort = 6969;
const uint32_t ID = 254 | ('R' << 8) | ('G' << 16) | ('B' << 24);

uint8_t rgbColor[] = {255, 0, 0};
uint8_t rgbLedPins[] = {D5, D6, D7};

bool fade = false;

WiFiUDP udp;
IPAddress broadcastIP(255, 255, 255, 255);
Preferences preferences;

void setup()
{
    const char* ssid;
    const char* password;

    for (byte i = 0; i < 3; i++)
    {
        pinMode(rgbLedPins[i], OUTPUT);
        analogWrite(rgbLedPins[i], 255 - rgbColor[i]);
    }

    NetworkCredentials cred = GetNetworkCredentials();

    ConnectToNetwork(WIFI_STA, cred);
}

NetworkCredentials GetNetworkCredentials()
{
    NetworkCredentials credentials;
    byte eepromVal = EEPROM.read(NetCredSetAddr);
    if (eepromVal == 0xFF) EEPROM.write(NetCredSetAddr, eepromVal = false); //only executes once over the lifetime of the mcu *normally anyway*
    if (eepromVal) return EEPROM.get(NetCredAddr, credentials);
    credentials = WaitNetworkCred();
    EEPROM.put(NetCredAddr, credentials);
    EEPROM.update(NetCredSetAddr, true);
    return 
}

NetworkCredentials WaitNetworkCred()
{
    const char* ssidAndPass = GetSsidAndPass();

    ConnectToNetwork(WIFI_AP, NetworkCredentials(ssidAndPass, ssidAndPass))

    while (true)
    {
        int packetSize = udp.parsePacket();
        if (packetSize)
        {
            char packetBuffer[255];
            int len = udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
            if (len > 0)
            {
                packetBuffer[len] = '\0';
                Serial.print("Received packet: ");
                Serial.println(packetBuffer);

                String receivedData = String(packetBuffer);
                int splitIndex = receivedData.indexOf(':');
                String receivedSsid = receivedData.substring(0, splitIndex);
                String receivedPassword = receivedData.substring(splitIndex + 1);
                return NetworkCredentials(receivedSsid.c_str(), receivedPassword.c_str());
            }
        }
    }
}

void ConnectToNetwork(WiFiMode mode, NetworkCredentials credentials)
{
    bool isAp = mode == WIFI_AP;
    Wifi.mode(mode);
    if (isAP) WiFi.softAP(credentials.Ssid, credentials.Password);
    else Wifi.begin(credentials.Ssid, credentials.Password);

    Serial.print(!isAP ? "Connecting to: " : "Starting AP: ");
    Serial.print(credentials.ssid);
    while (!isAp && WiFi.status() != WL_CONNECTED)
    {
        delay(350);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Success!");
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP() ? WiFi.softAPIP : WiFi.localIP());

    udp.begin(localUdpPort);
    Serial.print("UDP server started on port ");
    Serial.println(localUdpPort);
}

void loop()
{
    ListenForPackets();
    static unsigned long lastSampleTime = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastSampleTime >= SAMPLEDELAY_MINUTE * 60 * 1000)
    {
        SendControlSignal();
        lastSampleTime = currentTime;
    }
}

#error TODO: change name
void ListenForPackets()
{
    int packetSize = udp.parsePacket();
    if (packetSize)
    {
        char packetBuffer[255];
        int len = udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
        if (len > 0)
        {
            packetBuffer[len] = '\0';
            Serial.print("Received packet: ");
            Serial.println(packetBuffer);

            String packet = String(packetBuffer);
            if (packet.startsWith("#"))
            {
                int splitIndex = packet.indexOf('#', 1);
                String command = packet.substring(1, splitIndex);
                String argument = packet.substring(splitIndex + 1);
                ExecuteCommand(command, argument);
            }
        }
    }
}

void ExecuteCommand(const String& command, const String& argument)
{
    if (command.equals("SETCOLOR"))
    {
        SetColor(argument);
    }
    else if (command.equals("CYCLE"))
    {
        Cycle();
    }
}

void SetColor(const String& color)
{
    uint8_t r = strtol(color.substring(0, 2).c_str(), NULL, 16);
    uint8_t g = strtol(color.substring(2, 4).c_str(), NULL, 16);
    uint8_t b = strtol(color.substring(4, 6).c_str(), NULL, 16);

#error set arg below to byte array of r g and b
    SetColor();
}

void SetColor(const uint8_t* color)
{
    rgbColor[R] = color[R];
    rgbColor[G] = color[G];
    rgbColor[B] = color[B];

    analogWrite(rgbLedPins[R], 255 - rgbColor[R] );
    analogWrite(rgbLedPins[G], 255 - rgbColor[G] );
    analogWrite(rgbLedPins[B], 255 - rgbColor[B] );
}

void Cycle(bool cancel)
{
    uint16_t startTime = millis();
    for (short c = 0; !cancel && c <= 255; c++)
    {
        analogWrite(rgbLedPins[G], 255 - c);
        PrintCurrentColor();
        while (!cancel && startTime + 4 > millis())
        {
        }
    }

    startTime = millis();
    for (short c = 255; !cancel && c >= 0; c--)
    {
        analogWrite(rgbLedPins[R], 255 - c);
        PrintCurrentColor();
        while (!cancel && startTime + 4 > millis())
        {
        }
    }

    startTime = millis();
    for (short c = 0; !cancel && c <= 255; c++)
    {
        analogWrite(rgbLedPins[B], 255 - c);
        PrintCurrentColor();
        while (!cancel && startTime + 4 > millis())
        {
        }
    }

    startTime = millis();
    for (short c = 255; !cancel && c >= 0; c--)
    {
        analogWrite(rgbLedPins[G], 255 - c);
        PrintCurrentColor();
        while (!cancel && startTime + 4 > millis())
        {
        }
    }

    startTime = millis();
    for (short c = 0; !cancel && c <= 255; c++)
    {
        analogWrite(rgbLedPins[R], 255 - c);
        PrintCurrentColor();
        while (!cancel && startTime + 4 > millis())
        {
        }
    }

    startTime = millis();
    for (short c = 255; !cancel && c >= 0; c--)
    {
        analogWrite(rgbLedPins[B], 255 - c);
        PrintCurrentColor();
        while (!cancel && startTime + 4 > millis())
        {
        }
    }

    if (cancel) 
}

#error TODO: change name
void SendControlSignal() 
{
    // Prepare the control signal packet
    const int packetSize = 8;
    uint8_t packet[packetSize];
    packet[0] = 0x11;
    packet[1] = 'C';
    packet[2] = 'O';
    packet[3] = 'R';
    packet[4] = 'G';
    packet[5] = 'B';
    packet[6] = 0x1D;
    packet[7] = 0x01;

    udp.beginPacket(broadcastIP, localUdpPort);
    udp.write(packet, packetSize);
    udp.endPacket();
}
