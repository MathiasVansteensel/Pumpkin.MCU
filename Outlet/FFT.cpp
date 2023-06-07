#include "Complex.cpp"
#include "arduino.h"
#pragma once

#define PI 3.1415926535897932384626433832795

class FFT
{
public:
  Complex *Compute(Complex *signal, int signalLength)
  {
    if (signalLength <= 1)
      return signal;

    Complex *even = Compute(signal, signalLength / 2);
    Complex *odd = Compute(signal + 1, signalLength / 2);
    Complex *t = new Complex[signalLength / 2];

    for (int k = 0; k < signalLength / 2; k++)
    {
      float angle = -2 * PI * k / signalLength;
      //i would use a LUT for sin/cos for way better performance if i had more ram to work with
      Complex wk(cos(angle), sin(angle));
      t[k] = odd[k] * wk;
    }

    Complex *result = new Complex[signalLength];
    for (int k = 0; k < signalLength / 2; k++)
    {
      result[k] = even[k] + t[k];
      result[k + signalLength / 2] = even[k] - t[k];
    }

    delete[] even;
    delete[] odd;
    delete[] t;

    return result;
  }
};