#include "DHT.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define DHT_TYPE DHT22
#define DHT_PIN D5
#define LDR_PIN A0
#define ADC_RES (1<<10)-1 //10 bit
#define OP_VOLTAGE 3.3
#define LDR_DIV_RES 330
#define SAMPLEDELAY_MINUTE 0.1

const uint32_t ID = 255 | ('H'<<8) | ('T'<<16) | ('L'<<24) ;

char* ssid = "TP-LINK_E19E";
char* password = "84190277";
const int udpPort = 8888;

WiFiUDP udp;
IPAddress broadcastIP = IPAddress(255, 255, 255, 255);
DHT dht(DHT_PIN, DHT_TYPE, 15);

void setup()
{
  pinMode(LDR_PIN, INPUT);
  Serial.begin(115200);
  dht.begin();
  InitWifi(udp, ssid, password, udpPort);
}

void loop() 
{  
  static unsigned long lastMillis = 0;
  if (millis() >= (lastMillis + SAMPLEDELAY_MINUTE*60000)) 
  {
    lastMillis = millis();
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature(false);
    float heatIndex = dht.computeHeatIndex(temperature, humidity, false);
    float lux = ComputeLux();
  
    Serial.println(String(lastMillis) + "ms | Sending > [L:" + String(lux) + "|T:" + String(temperature) + "|H:" + String(humidity) + "|I:" + String(heatIndex) + "]"); 
    udp.beginPacket(broadcastIP, udpPort);
    UdpWrite32(ID);
    udp.write('L');
    UdpWriteFloat(lux);
    udp.write('T');
    UdpWriteFloat(temperature);
    udp.write('H');
    UdpWriteFloat(humidity);
    udp.write('I');
    UdpWriteFloat(heatIndex);
    udp.endPacket();
  }
}

float ComputeLux() //reference values (approximation): https://cdn.shopify.com/s/files/1/2261/0117/files/luxled.jpg?v=1619002356 and https://www.cined.com/shedding-light-lumens-lux-latitude/
{
  float ldrVoltage = abs((analogRead(LDR_PIN)/ (float)ADC_RES) * 3.3);
  float ldrResistance = (ldrVoltage * LDR_DIV_RES) / (OP_VOLTAGE - ldrVoltage);
  float ldrLux = (float)6659854143.996064 * (float)pow(ldrResistance, -2.004023338209067); 

  return ldrLux;
}
