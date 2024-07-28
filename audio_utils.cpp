#include "audio_utils.h"
#include "callback_data.h"
#include "note_detector.h"

#include <portaudio.h>
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

int processAudio(
    const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData
    ) {

    CallbackData* data = static_cast<CallbackData*>(userData);
    float* in = (float*)inputBuffer;
    if (inputBuffer == NULL) {
        return paContinue;
    }

    // Generate Hamming window
    std::vector<double> hammingWindow = generateHammingWindow(framesPerBuffer);

    // Apply Hamming window to input buffer and copy to FFT input array
    for (unsigned long i = 0; i < framesPerBuffer; i++) {
        data->fftInput[i] = static_cast<double>(in[i]) * hammingWindow[i];
    }

    // Execute the FFT plan
    fftw_execute(reinterpret_cast<fftw_plan>(data->fftPlan));

    // Get dominant frequency
    double frequency = getFrequency(reinterpret_cast<fftw_complex*>(data->fftOutput), framesPerBuffer);
    if (frequency <= 0) {
        return paContinue;
    }

    // Check if the input is significant
    double maxAmplitude = 0;
    for (unsigned long i = 0; i < framesPerBuffer; i++) {
        if (fabs(static_cast<double>(data->fftInput[i])) > maxAmplitude) {
            maxAmplitude = fabs(static_cast<double>(data->fftInput[i]));
        }
    }

    if (maxAmplitude < 0.1) {
        return paContinue; // Ignore low-amplitude input
    }

    // Convert frequency to note name
    std::string noteName;

    try {
        noteName = freqToNoteName(frequency);
    } catch (const std::exception& e) {
        std::cerr << "Error converting frequency to note name: " << e.what() << std::endl;
        return paContinue;
    }

    std::cout << "Detected note: " << noteName << std::endl;

    return paContinue;
}