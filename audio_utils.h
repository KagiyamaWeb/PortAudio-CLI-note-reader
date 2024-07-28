//#pragma once
#ifndef AUDIO_UTILS_H
#define AUDIO_UTILS_H

#include <portaudio.h>
#include <cmath>
#include <vector>

std::vector<double> generateHammingWindow(unsigned long size);
int processAudio(
    const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);

#endif // AUDIO_UTILS_H