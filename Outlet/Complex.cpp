#include "math.h"

#pragma once

struct Complex
{
public:
    float Real = 0;

public:
    float Imaginary = 0;

public:
    Complex();

public:
    Complex(float real, float imag)
    {
        Real = real;
        Imaginary = imag;
    }

public:
    float ComputeMagnitude()
    {
        return sqrtf(Real * Real + Imaginary * Imaginary);
    }

public:
    Complex operator+(const Complex &b) const
    {
        return Complex(Real + b.Real, Imaginary + b.Imaginary);
    }

public:
    Complex operator-(const Complex &b) const
    {
        return Complex(Real - b.Real, Imaginary - b.Imaginary);
    }

public:
    Complex operator*(const Complex &b) const
    {
        float real = Real * b.Real - Imaginary * b.Imaginary;
        float imag = Real * b.Imaginary + Imaginary * b.Real;
        return Complex(real, imag);
    }

public:
    Complex operator/(const Complex &b) const
    {
        float denominator = b.Real * b.Real + b.Imaginary * b.Imaginary;
        float real = (Real * b.Real + Imaginary * b.Imaginary) / denominator;
        float imag = (Imaginary * b.Real - Real * b.Imaginary) / denominator;
        return Complex(real, imag);
    }

public:
    bool operator==(const Complex &b) const
    {
        return Real == b.Real && Imaginary == b.Imaginary;
    }

public:
    bool operator!=(const Complex &b) const
    {
        return !(operator==(b));
    }
};
