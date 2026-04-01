#include "note_detector.h"
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

constexpr double A4_FREQ = 440.0;
constexpr int A4_INDEX = 9;
constexpr int NUM_NOTES = 12;

double getFrequency(fftw_complex* fftOutput, int numSamples, double sampleRate) {
    int maxIndex = 0;
    double maxMagnitude = 0.0;

    for (int i = 0; i < numSamples / 2 + 1; ++i) {
        double magnitude = sqrt(fftOutput[i][0] * fftOutput[i][0] + fftOutput[i][1] * fftOutput[i][1]);
        if (magnitude > maxMagnitude) {
            maxMagnitude = magnitude;
            maxIndex = i;
        }
    }

    return maxIndex * sampleRate / numSamples;
}

std::string freqToNoteName(double frequency) {
    const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

    int noteIndex = static_cast<int>(round(12 * log2(frequency / A4_FREQ)) + A4_INDEX);

    const char* noteName;
    int octave;
    if (noteIndex < 0) {
        noteName = noteNames[NUM_NOTES + noteIndex % NUM_NOTES];
        octave = floor(noteIndex / NUM_NOTES) + 3;
    } else {
        noteName = noteNames[noteIndex % NUM_NOTES];
        octave = noteIndex / NUM_NOTES + 4;
    }

    return std::string(noteName) + std::to_string(octave);
}