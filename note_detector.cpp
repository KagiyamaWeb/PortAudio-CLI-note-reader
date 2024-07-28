#include "note_detector.h"
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#define a4Freq 440.0
#define a4Index 9 // Index of A in noteNames
#define numNotes 12

double getFrequency(fftw_complex* fftOutput, int numSamples) {
    int maxIndex = 0;
    double maxMagnitude = 0.0;

    for (int i = 0; i < numSamples / 2 + 1; ++i) {
        double magnitude = sqrt(fftOutput[i][0] * fftOutput[i][0] + fftOutput[i][1] * fftOutput[i][1]);
        if (magnitude > maxMagnitude) {
            maxMagnitude = magnitude;
            maxIndex = i;
        }
    }

    double frequency = maxIndex * (double)SAMPLE_RATE / numSamples;
    return frequency;
}

std::string freqToNoteName(double frequency) {
    // Define frequencies for standard A440 tuning
    const char* noteNames[] = {"C", "C/#", "D", "D/#", "E", "F", "F/#", "G", "G/#", "A", "A/#", "B"};

    std::cout << "frequency: " << frequency << std::endl;

    int noteIndex = static_cast<int>(round(12 * log2(frequency / a4Freq)) + a4Index);
    std::cout << "noteIndex: " << noteIndex << std::endl;

    const char* noteName;
    int octave;
    if (noteIndex < 0) {
        noteName = noteNames[numNotes + noteIndex % numNotes];
        octave = floor(noteIndex / numNotes) + 3;
    }
    else {
        noteName = noteNames[noteIndex % numNotes];
        octave = noteIndex / numNotes + 4;
    }

    return std::string(noteName) + std::to_string(octave);
}