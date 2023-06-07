#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include "Pumpkin.MCU.Common.h"

#define UDP_TX_PACKET_MAX_SIZE 8192
#define R 0
#define G 1
#define B 2
#define SAMPLEDELAY_MINUTE 0.5
#define NetCredSetAddr 0x0
#define NetCredAddr 0x69
#define MODULE_TYPE "PumpkinSmartLamp"

const uint8_t guid[16] = { 0x73, 0xB8, 0x3B, 0xD, 0x21, 0x35, 0x1C, 0x43, 0xBC, 0x35, 0x1, 0x85, 0xA4, 0x5A, 0x56, 0x27 };
const uint16_t localUdpPort = 6969;

uint8_t rgbColor[] = {0, 0, 0};
uint8_t rgbLedPins[] = {D5, D6, D7};

bool fade = false;

WiFiUDP udp;
IPAddress broadcastIP(255, 255, 255, 255);

void setup()
{
    for (byte i = 0; i < 3; i++)
    {
        pinMode(rgbLedPins[i], OUTPUT);
        analogWrite(rgbLedPins[i], 255 - rgbColor[i]);
    }

    Serial.begin(115200);
    Serial.println("SETUP");

    Serial.println("getting credentials");
    GetNetworkCredentials();
    Serial.println("done");
}

void loop()
{
    ListenUdp();
    static unsigned long lastSampleTime = 0;
    unsigned long currentTime = millis();

    if (fade) Fade();
    else SetColor(rgbColor);

    if (currentTime - lastSampleTime >= SAMPLEDELAY_MINUTE * 60 * 1000)
    {
        SendDataToHub();
        lastSampleTime = currentTime;
    }
}

void ListenUdp()
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
        fade = false;
        SetColor(argument);
        Serial.print("Command: set color to #");
        Serial.println(argument);
    }
    else if (command.equals("CYCLE")) 
    {
        fade = !fade;
        Serial.println("Command: cycle");
    }
}

void SetColor(const String& color)
{
    uint8_t r = strtol(color.substring(0, 2).c_str(), NULL, 16);
    uint8_t g = strtol(color.substring(2, 4).c_str(), NULL, 16);
    uint8_t b = strtol(color.substring(4, 6).c_str(), NULL, 16);

    const uint8_t rgbVals[] = {r, g, b};

    SetColor(rgbVals);
}

void SetColor(const uint8_t* color)
{
    bool colorChanged = false;
    for (size_t i = 0; i < 3; i++) 
    {
        if (rgbColor[i] != color[i]) 
        {
          colorChanged = true;
          break;
        }
    }

    if (colorChanged)
    {
        Serial.print("Set lamp color to: R(");
        Serial.print(color[R]);
        Serial.print(") G(");
        Serial.print(color[G]);
        Serial.print(") B(");
        Serial.print(color[B]);
        Serial.println(')');
    }

    rgbColor[R] = color[R];
    rgbColor[G] = color[G];
    rgbColor[B] = color[B];

    analogWrite(rgbLedPins[R], 255 - rgbColor[R] );
    analogWrite(rgbLedPins[G], 255 - rgbColor[G] );
    analogWrite(rgbLedPins[B], 255 - rgbColor[B] );
}

void Fade()
{
    uint16_t startTime = millis();
    for (short c = 0; c <= 255; c++)
    {
        analogWrite(rgbLedPins[G], 255 - c);
        while (startTime + 4 > millis()) ListenUdp();
    }

    startTime = millis();
    for (short c = 255; c >= 0; c--)
    {
        analogWrite(rgbLedPins[R], 255 - c);
        while (startTime + 4 > millis()) ListenUdp();
    }

    startTime = millis();
    for (short c = 0; c <= 255; c++)
    {
        analogWrite(rgbLedPins[B], 255 - c);
        while (startTime + 4 > millis()) ListenUdp();
    }

    startTime = millis();
    for (short c = 255; c >= 0; c--)
    {
        analogWrite(rgbLedPins[G], 255 - c);
        while (startTime + 4 > millis()) ListenUdp();
    }

    startTime = millis();
    for (short c = 0; c <= 255; c++)
    {
        analogWrite(rgbLedPins[R], 255 - c);
        while (startTime + 4 > millis()) ListenUdp();
    }

    startTime = millis();
    for (short c = 255; c >= 0; c--)
    {
        analogWrite(rgbLedPins[B], 255 - c);
        while (startTime + 4 > millis()) ListenUdp();
    }
}

void SendDataToHub() 
{
    const int packetSize = 29;
    int startIndex = 0;
    uint8_t packet[packetSize];
    packet[startIndex++] = 0x11;
    for (int i = 0; i < 16; i++) packet[startIndex++] = guid[i];
    packet[startIndex++] = 'O';
    packet[startIndex++] = 'C';
    packet[startIndex++] = 'R';
    packet[startIndex++] = 'G';
    packet[startIndex++] = 'B';
    packet[startIndex++] = 0x1D;
    packet[startIndex++] = (rgbColor[0] | rgbColor[1] | rgbColor[2]);
    packet[startIndex++] = (uint8_t)fade;
    packet[startIndex++] = rgbColor[0];
    packet[startIndex++] = rgbColor[1];
    packet[startIndex++] = rgbColor[2];

    udp.beginPacket(broadcastIP, localUdpPort);
    udp.write(packet, packetSize);
    udp.endPacket();
    Serial.print("sent: ");
    for (int i = 0; i < packetSize; i++)
    {
        Serial.print(packet[i]);
        Serial.print(" ");
    } 
    Serial.println();
}
