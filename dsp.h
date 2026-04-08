#ifndef DSP_H
#define DSP_H

#include <vector>

double runFFTDetection(const double* samples, size_t sampleCount, size_t fftSize, double sampleRate);

double runAutocorrelationPitch(const double* samples, size_t sampleCount, double sampleRate, 
                              double minFreq, double maxFreq);

double runYinPitch(const double* samples, size_t sampleCount, double sampleRate,
                   double threshold);

double runCepstrumPitch(const double* samples, size_t sampleCount, size_t fftSize, 
                        double sampleRate, double minFreq, double maxFreq);

#endif
