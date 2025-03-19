#pragma once
#include <cmath>
#include <iostream>
#include <vector>

static const double M_PI = 3.14159265358979323846;

class HighShelfFilter
{
    private:
        double a0, a1, a2, b0, b1, b2;
        double prevX1 = 0.0, prevX2 = 0.0;
        double prevY1 = 0.0, prevY2 = 0.0;

    public:
        HighShelfFilter(double sampleRate, double cutoffFreq, double gainDB, double Q = 0.707)
        {
            setCoefficients(sampleRate, cutoffFreq, gainDB, Q);
        }

        void setCoefficients(double sampleRate, double cutoffFreq, double gainDB, double Q)
        {
            double A = std::pow(10.0, gainDB / 40.0); // Convert dB to linear gain
            double omega = 2.0 * M_PI * cutoffFreq / sampleRate;
            double sinOmega = std::sin(omega);
            double cosOmega = std::cos(omega);
            double alpha = sinOmega / (2.0 * Q);
            double beta = std::sqrt(A + A);

            // Coefficients for high shelf filter (RBJ formula)
            b0 = A * ((A + 1) + (A - 1) * cosOmega + beta * sinOmega);
            b1 = -2.0 * A * ((A - 1) + (A + 1) * cosOmega);
            b2 = A * ((A + 1) + (A - 1) * cosOmega - beta * sinOmega);
            a0 = (A + 1) - (A - 1) * cosOmega + beta * sinOmega;
            a1 = 2.0 * ((A - 1) - (A + 1) * cosOmega);
            a2 = (A + 1) - (A - 1) * cosOmega - beta * sinOmega;

            // Normalize coefficients
            b0 /= a0;
            b1 /= a0;
            b2 /= a0;
            a1 /= a0;
            a2 /= a0;
        }

        double process(double inputSample)
        {
            double outputSample = (b0 * inputSample) + (b1 * prevX1) + (b2 * prevX2) - (a1 * prevY1) - (a2 * prevY2);

            // Update states
            prevX2 = prevX1;
            prevX1 = inputSample;
            prevY2 = prevY1;
            prevY1 = outputSample;

            return outputSample;
        }
};