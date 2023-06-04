// #ifdef ESP8266
// #include <ESP8266WiFi.h>
// #endif

// #ifdef ESP32
// #include <WiFi.h>
// #endif

// //So glad both use the same udp lib
// #include <WiFiUdp.h>

// const unsigned int DefaultPort = 6969;

// class Network 
// {
// private:
//   WiFiUDP udp;
//   unsigned int port;
//   bool initialized;

// public:
//   typedef void (*DatagramReceivedHandler)(byte[], uint16_t, IPAddress);

//   DatagramReceivedHandler DatagramReceived;

//   Network() 
//   {
//     initialized = false;
//     port = DefaultPort;
//   }

//   void Initialize(unsigned int recvPort = DefaultPort) 
//   {
//     if (initialized) return;

//     port = recvPort;
//     udp.begin(port);
//     initialized = true;
//   }

//   void Receive() 
//   {
//     ReceiveCallback();
//   }

//   void Send(const String& message, const IPAddress& ipAddress, unsigned int port) 
//   {
//     udp.beginPacket(ipAddress, port);
//     udp.write((const uint8_t*)message.c_str(), message.length());
//     udp.endPacket();
//   }

// private:
//   void ReceiveCallback() 
//   {
//     int packetSize = udp.parsePacket();
//     if (packetSize) 
//     {
      
//       IPAddress senderIp = udp.remoteIP();
//       if (udp.read() == 0x11)
//       {
//         byte* data = new byte[packetSize - 1];
//         udp.read(data, packetSize - 1);

//         if (DatagramReceived != nullptr) 
//         {
//           DatagramReceived(data, packetSize, senderIp);
//         }
//         delete[] data;
//       }
//     }

//   }
// };
