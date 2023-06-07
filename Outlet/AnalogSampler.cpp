// #include "arduino.h"
// #pragma once

// class AnalogSampler {
//   private:
//     float samplePeriodMicros;
//     uint8_t *pins;
//     uint8_t pinCount;
//     uint16_t sampleCount;
//   public:
//     AnalogSampler(float sampleFreq, uint16_t samplecount, uint8_t pins[]) 
//     {
//       samplePeriodMicros = 1000000.0f / sampleFreq, 
//       this->pinCount = sizeof(pins); //friendly reminder to future me: pinCount is already 8bits
//       this->pins = pins;
//       this->sampleCount = sampleCount;
//     }
    
//     //rip ADC. The ADC has some recovery time (+/-1ms) but it should be fine
//     void sample(int16_t *sampleBuffer[pinCount][sampleCount])
//     {
//       for (uint32_t i = 0; i < sampleCount; i++)
//       {
//         for (uint8_t p = 0; p < pinCount; p++) 
//         {
//           sampleBuffer[p][i] = analogRead(pins[i]);
//         }
//         delayMicroseconds(samplePeriodMicros);
//       }
//     }
// };