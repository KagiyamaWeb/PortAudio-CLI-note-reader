#ifndef NOTE_DETECTOR_H
#define NOTE_DETECTOR_H

#include <fftw3.h>
#include <cmath>
#include <string>

// Unified sample rate - must match audio capture rate
constexpr double SAMPLE_RATE = 48000.0;
constexpr int FRAMES_PER_BUFFER = 512;

double getFrequency(fftw_complex* fftOutput, int numSamples, double sampleRate);
std::string freqToNoteName(double frequency);

#endif // NOTE_DETECTOR_H
