#include <WiFiUdp.h>
#include "WiFi.h"
#include "Pumpkin.MCU.Common.h"
#include <EEPROM.h>
#include "FFT.cpp"
#include "Complex.cpp"
#include <Adafruit_ADS1X15.h>

#define SAMPLEDELAY_MINUTE 0.1
#define UDP_TX_PACKET_MAX_SIZE 8192
#define NetCredSetAddr 0x0
#define NetCredAddr 0x69
#define SampleSetSize 2048
#define MODULE_TYPE "PumpkinSmartOutlet"

const int switchPin = 32;

const uint8_t guid[16] = { 0x49, 0x37, 0xB5, 0x4E, 0xB7, 0x7, 0xE2, 0x4C, 0xAB, 0x90, 0xF1, 0xFB, 0x9, 0x86, 0xEB, 0x93 };
const uint16_t localUdpPort = 6969;

float current, voltage, powerFactor, power;
bool state = true;

WiFiUDP udp;
Adafruit_ADS1015 ads; 
IPAddress broadcastIP = IPAddress(255, 255, 255, 255);

void setup()
{
  Serial.begin(115200);
  Serial.println("SETUP");

  Serial.println("getting credentials");
  GetNetworkCredentials();
  Serial.println("done");

  pinMode(switchPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(switchPin), ToggleMain, CHANGE);

  if (!ads.begin()) 
  {
    Serial.println("Failed to initialize ADS.");
    while (true)
    {
      digitalWrite(LED_BUILTIN, HIGH); 
      delay(500);
      digitalWrite(LED_BUILTIN, LOW); 
      delay(500);
    }
  }

  Serial.println("LOOP");
}

void ToggleMain()
{
  state = !state;
}

void ReadVoltageAndCurrent(uint16_t voltageArray[] /*, float& currentArray[]*/)
{
  int valCount = 0;
  while (valCount < SampleSetSize)
  {
    voltageArray[valCount] = ads.readADC_SingleEnded(0);
    currentArray[i] = ads.readADC_SingleEnded(1);
    valCount++;
  }
}

void loop()
{
  static unsigned long lastMillis = 0;
  if (millis() >= (lastMillis + SAMPLEDELAY_MINUTE * 60000))
  {
    uint16_t voltageArray[SampleSetSize];
    ReadVoltageAndCurrent(voltageArray);
    int max = 0;
    for (int i = 0; i < SampleSetSize; i++) 
    {
      int currentVal = voltageArray[i];
      max = currentVal > max ? currentVal : max;      
    }
    maxVal /= (1<<16)-1;
    maxVal *= 325.2691193458119;
    voltage = (float)maxVal/sqrtf(2)
  }
}

void SendDataToHub() 
{
    const int packetSize = 40;
    int startIndex = 0;
    uint8_t packet[packetSize];
    packet[startIndex++] = 0x11;
    for (int i = 0; i < 16; i++) packet[startIndex++] = guid[i];
    packet[startIndex++] = 'I';
    packet[startIndex++] = 'V';
    packet[startIndex++] = 'P';
    packet[startIndex++] = 'F';
    packet[startIndex++] = 0x1D;

    // uint8_t* tempBytes = (uint8_t*)&temperature;
    // for (int i = 0; i < 4; i++) packet[startIndex++] = tempBytes[i];

    // uint8_t* humBytes = (uint8_t*)&humidity;
    // for (int i = 0; i < 4; i++) packet[startIndex++] = humBytes[i];

    // uint8_t* heatBytes = (uint8_t*)&heatIndex;
    // for (int i = 0; i < 4; i++) packet[startIndex++] = heatBytes[i];

    // uint8_t* luxBytes = (uint8_t*)&lux;
    // for (int i = 0; i < 4; i++) packet[startIndex++] = luxBytes[i];

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