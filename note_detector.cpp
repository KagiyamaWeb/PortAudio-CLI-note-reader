#include "note_detector.h"

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
    const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    int numNotes = sizeof(noteNames) / sizeof(noteNames[0]);

    double a4Freq = 440.0;
    int a4Index = 9; // Index of A in noteNames

    int noteIndex = (int)(round(12 * log2(frequency / a4Freq)) + a4Index);
    int octave = noteIndex / numNotes - 1;
    const char* noteName = noteNames[noteIndex % numNotes];

    return std::string(noteName) + std::to_string(octave);
}