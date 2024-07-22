#pragma once
#ifndef AUDIO_UTILS_H
#define AUDIO_UTILS_H

#include <portaudio.h>
#include "note_detector.h"

double hammingWindow(int n, int N) {
    return 0.54 - 0.46 * cos(2 * M_PI * n / (N - 1));
}

int processAudio(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);

#endif // AUDIO_UTILS_H