#include "DHT.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "Pumpkin.MCU.Common.h"

#define DHT_TYPE DHT22
#define DHT_PIN D5
#define LDR_PIN A0
#define ADC_RES (1<<10)-1 //10 bit
#define OP_VOLTAGE 3.3
#define LDR_DIV_RES 330
#define SAMPLEDELAY_MINUTE 0.1
#define NetCredSetAddr 0x0
#define NetCredAddr 0x69
#define MODULE_TYPE "PumpkinSensorStation"

const uint8_t guid[16] = { 0xE0, 0xED, 0x7B, 0x17, 0x40, 0xC2, 0x87, 0x4E, 0xA2, 0xE2, 0x2E, 0x29, 0xCE, 0x67, 0x12, 0xA2 };
const uint16_t localUdpPort = 6969;

WiFiUDP udp;
IPAddress broadcastIP = IPAddress(255, 255, 255, 255);
DHT dht(DHT_PIN, DHT_TYPE, 15);

float humidity, temperature, heatIndex, lux;

void setup()
{
  Serial.begin(115200);
  Serial.println("SETUP");

  Serial.println("getting credentials");
  GetNetworkCredentials();
  Serial.println("done");
  
  Serial.println("setting up pins");
  pinMode(LDR_PIN, INPUT);  
  dht.begin();

  Serial.println("LOOP");
}

void loop() 
{  
  static unsigned long lastMillis = 0;
  if (millis() >= (lastMillis + SAMPLEDELAY_MINUTE*60000)) 
  {
    lastMillis = millis();
    humidity = dht.readHumidity();
    temperature = dht.readTemperature(false);
    heatIndex = dht.computeHeatIndex(temperature, humidity, false);
    lux = ComputeLux();
    Serial.println("Sending data");
    SendDataToHub();
  }
}

void SendDataToHub() 
{
    const int packetSize = 40;
    int startIndex = 0;
    uint8_t packet[packetSize];
    packet[startIndex++] = 0x11;
    for (int i = 0; i < 16; i++) packet[startIndex++] = guid[i];
    packet[startIndex++] = 'T';
    packet[startIndex++] = 'H';
    packet[startIndex++] = 'I';
    packet[startIndex++] = 'L';
    packet[startIndex++] = 0x1D;

    uint8_t* tempBytes = (uint8_t*)&temperature;
    for (int i = 0; i < 4; i++) packet[startIndex++] = tempBytes[i];

    uint8_t* humBytes = (uint8_t*)&humidity;
    for (int i = 0; i < 4; i++) packet[startIndex++] = humBytes[i];

    uint8_t* heatBytes = (uint8_t*)&heatIndex;
    for (int i = 0; i < 4; i++) packet[startIndex++] = heatBytes[i];

    uint8_t* luxBytes = (uint8_t*)&lux;
    for (int i = 0; i < 4; i++) packet[startIndex++] = luxBytes[i];

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

float ComputeLux() //reference values (approximation): https://cdn.shopify.com/s/files/1/2261/0117/files/luxled.jpg?v=1619002356 and https://www.cined.com/shedding-light-lumens-lux-latitude/
{
  float ldrVoltage = abs((analogRead(LDR_PIN)/ (float)ADC_RES) * 3.3);
  float ldrResistance = (ldrVoltage * LDR_DIV_RES) / (OP_VOLTAGE - ldrVoltage);
  float ldrLux = (float)6659854143.996064 * (float)pow(ldrResistance, -2.004023338209067); 

  return ldrLux;
}