#ifndef AUDIO_UTILS_H
#define AUDIO_UTILS_H

#include <vector>
#include <mutex>
#include <portaudio.h>

class FixedBuffer {
public:
    FixedBuffer(size_t size);
    void put(double sample);
    bool isFull() const;
    void reset();
    std::vector<double> getBuffer() const;

private:
    std::vector<double> buffer;
    size_t writeIndex = 0;
    mutable std::mutex mtx;
};

std::vector<double> generateHammingWindow(unsigned long size);

class LowPassFilter {
public:
    LowPassFilter(double cutoffFreq, double sampleRate);
    double process(double sample);
private:
    double alpha;
    double prevOutput = 0.0;
};

int processAudio(const void* inputBuffer, void* outputBuffer,
                unsigned long framesPerBuffer,
                const PaStreamCallbackTimeInfo* timeInfo,
                PaStreamCallbackFlags statusFlags,
                void* userData);

#endif