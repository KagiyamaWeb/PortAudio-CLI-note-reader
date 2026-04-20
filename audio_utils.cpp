#include "audio_utils.h"
#include "dsp.h"
#include "note_detector.h"

#include <portaudio.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <mutex>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

constexpr double NOISE_THRESHOLD = 0.02;
constexpr size_t FFT_BUFFER_SIZE = 8192;

std::vector<double> generateHammingWindow(unsigned long size) {
    std::vector<double> window(size);
    for (unsigned long i = 0; i < size; ++i) {
        window[i] = 0.54 - 0.46 * cos(2 * M_PI * i / (size - 1));
    }
    return window;
}

LowPassFilter::LowPassFilter(double cutoff, double sr) {
    double rc = 1.0 / (2 * M_PI * cutoff);
    double dt = 1.0 / sr;
    alpha = dt / (rc + dt);
}

double LowPassFilter::process(double sample) {
    double output = alpha * sample + (1 - alpha) * prevOutput;
    prevOutput = output;
    return output;
}

FixedBuffer::FixedBuffer(size_t size) : buffer(size) {}

void FixedBuffer::put(double sample) {
    std::lock_guard<std::mutex> lock(mtx);
    if (writeIndex < buffer.size()) {
        buffer[writeIndex++] = sample;
    }
}

bool FixedBuffer::isFull() const {
    std::lock_guard<std::mutex> lock(mtx);
    return writeIndex >= buffer.size();
}

void FixedBuffer::reset() {
    std::lock_guard<std::mutex> lock(mtx);
    writeIndex = 0;
}

std::vector<double> FixedBuffer::getBuffer() const {
    std::lock_guard<std::mutex> lock(mtx);
    return buffer;
}

FixedBuffer fixedBuffer(FFT_BUFFER_SIZE);
static LowPassFilter lpf(LOWPASS_CUTOFF, SAMPLE_RATE);

int processAudio(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
                 const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
    (void)outputBuffer;
    (void)timeInfo;
    (void)statusFlags;
    (void)userData;

    if (inputBuffer == NULL) {
        return paContinue;
    }

    float* in = (float*)inputBuffer;

    double maxAmplitude = 0;
    for (unsigned long i = 0; i < framesPerBuffer; i++) {
        if (fabs(static_cast<double>(in[i])) > maxAmplitude) {
            maxAmplitude = fabs(static_cast<double>(in[i]));
        }
    }

    if (maxAmplitude < NOISE_THRESHOLD) {
        return paContinue;
    }

    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        double filtered = lpf.process(in[i]);
        fixedBuffer.put(filtered);
    }

    if (fixedBuffer.isFull()) {
        auto fftInput = fixedBuffer.getBuffer();
        fixedBuffer.reset();

        double frequency = runAutocorrelationPitch(fftInput.data(), fftInput.size(), SAMPLE_RATE, 40, LOWPASS_CUTOFF);

        if (frequency <= 0) {
            return paContinue;
        }

        std::string noteName = freqToNoteName(frequency);
        std::cout << "Detected note: " << noteName << " : " << frequency << std::endl;
    }

    return paContinue;
}
