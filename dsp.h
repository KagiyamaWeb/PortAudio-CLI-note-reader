#ifndef DSP_H
#define DSP_H

#include <vector>

double runFFTDetection(const double* samples, size_t sampleCount, size_t fftSize, double sampleRate);

#endif
