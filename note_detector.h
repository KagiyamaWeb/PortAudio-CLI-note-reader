#ifndef NOTE_DETECTOR_H
#define NOTE_DETECTOR_H

#include <fftw3.h>
#include <cmath>
#include <string>

const int SAMPLE_RATE = 44100;
const int FRAMES_PER_BUFFER = 512;

double getFrequency(fftw_complex* fftOutput, int numSamples);
std::string freqToNoteName(double frequency);

#endif // NOTE_DETECTOR_H
