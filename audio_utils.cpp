#include "audio_utils.h"
#include "callback_data.h"
#include <iostream>
#include <fftw3.h>
#include <cmath>
#include <vector>


// Function to generate Hamming window
std::vector<double> generateHammingWindow(unsigned long size) {
    std::vector<double> window(size);
    for (unsigned long i = 0; i < size; ++i) {
        window[i] = 0.54 - 0.46 * cos(2 * M_PI * i / (size - 1));
    }
    return window;
}

static int processAudio(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
    auto* data = static_cast<CallbackData*>(userData);
    float* in = (float*)inputBuffer;
    if (inputBuffer == nullptr) {
        return paContinue;
    }

    // Generate Hamming window
    std::vector<double> hammingWindow = generateHammingWindow(framesPerBuffer);

    // Apply Hamming window to input buffer and copy to FFT input array
    for (unsigned long i = 0; i < framesPerBuffer; i++) {
        data->fftInput[i] = in[i] * hammingWindow[i];
    }

    // Execute the FFT plan
    fftw_execute(data->fftPlan);

    // Get dominant frequency
    double frequency = getFrequency(data->fftOutput, framesPerBuffer);

    // Check if the input is significant
    double maxAmplitude = 0;
    for (unsigned long i = 0; i < framesPerBuffer; i++) {
        if (fabs(data->fftInput[i]) > maxAmplitude) {
            maxAmplitude = fabs(data->fftInput[i]);
        }
    }
    if (maxAmplitude < 0.01) {
        return paContinue; // Ignore low-amplitude input
    }

    // Convert frequency to note name
    std::string noteName = freqToNoteName(frequency);

    // Print note name to console
    std::cout << "Detected note: " << noteName << std::endl;

    return paContinue;
}