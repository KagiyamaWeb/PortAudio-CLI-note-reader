#define _USE_MATH_DEFINES
#include "note_detector.h"
#include "audio_utils.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <fftw3.h>
#include <cassert>
#include <sstream>

static int testsPassed = 0;
static int testsFailed = 0;

#define TEST(name, expr) do { \
    if (expr) { \
        std::cout << "[PASS] " << name << "\n"; \
        testsPassed++; \
    } else { \
        std::cout << "[FAIL] " << name << "\n"; \
        testsFailed++; \
    } \
} while(0)

double runFFTDetection(const std::vector<double>& signal, double sampleRate, size_t fftSize) {
    std::vector<double> windowed(fftSize);
    size_t copySize = std::min(signal.size(), fftSize);
    for (size_t i = 0; i < copySize; ++i) {
        windowed[i] = signal[i];
    }

    auto hamming = generateHammingWindow(fftSize);
    for (size_t i = 0; i < fftSize; ++i) {
        windowed[i] *= hamming[i];
    }

    fftw_complex* fftOutput = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fftSize / 2 + 1));
    fftw_plan plan = fftw_plan_dft_r2c_1d(fftSize, windowed.data(), fftOutput, FFTW_ESTIMATE);
    fftw_execute(plan);

    double freq = getFrequency(fftOutput, fftSize, sampleRate);

    fftw_destroy_plan(plan);
    fftw_free(fftOutput);

    return freq;
}

void testFrequencyDetection440Hz() {
    constexpr double TARGET_FREQ = 440.0;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t FFT_SIZE = 4096;

    std::vector<double> signal(FFT_SIZE);
    for (size_t i = 0; i < FFT_SIZE; ++i) {
        signal[i] = sin(2 * M_PI * TARGET_FREQ * i / SAMPLE_RATE);
    }

    double detected = runFFTDetection(signal, SAMPLE_RATE, FFT_SIZE);
    double error = fabs(detected - TARGET_FREQ);

    std::ostringstream msg;
    msg << "440Hz detection (error: " << error << "Hz)";
    TEST(msg.str(), error < 1.0);
}

void testFrequencyDetection523Hz() {
    constexpr double TARGET_FREQ = 523.25;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t FFT_SIZE = 4096;

    std::vector<double> signal(FFT_SIZE);
    for (size_t i = 0; i < FFT_SIZE; ++i) {
        signal[i] = sin(2 * M_PI * TARGET_FREQ * i / SAMPLE_RATE);
    }

    double detected = runFFTDetection(signal, SAMPLE_RATE, FFT_SIZE);
    double error = fabs(detected - TARGET_FREQ);

    std::ostringstream msg;
    msg << "523Hz detection (error: " << error << "Hz)";
    TEST(msg.str(), error < 1.0);
}

void testFrequencyDetection659Hz() {
    constexpr double TARGET_FREQ = 659.25;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t FFT_SIZE = 4096;

    std::vector<double> signal(FFT_SIZE);
    for (size_t i = 0; i < FFT_SIZE; ++i) {
        signal[i] = sin(2 * M_PI * TARGET_FREQ * i / SAMPLE_RATE);
    }

    double detected = runFFTDetection(signal, SAMPLE_RATE, FFT_SIZE);
    double error = fabs(detected - TARGET_FREQ);

    std::ostringstream msg;
    msg << "659Hz detection (error: " << error << "Hz)";
    TEST(msg.str(), error < 1.0);
}

void testFrequencyDetectionLow() {
    constexpr double TARGET_FREQ = 100.0;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t FFT_SIZE = 4096;

    std::vector<double> signal(FFT_SIZE);
    for (size_t i = 0; i < FFT_SIZE; ++i) {
        signal[i] = sin(2 * M_PI * TARGET_FREQ * i / SAMPLE_RATE);
    }

    double detected = runFFTDetection(signal, SAMPLE_RATE, FFT_SIZE);
    double error = fabs(detected - TARGET_FREQ);

    std::ostringstream msg;
    msg << "100Hz detection (error: " << error << "Hz)";
    TEST(msg.str(), error < 1.0);
}

void testNoteNameA4() {
    std::string note = freqToNoteName(440.0);
    TEST("A4 note name", note == "A4");
}

void testNoteNameC4() {
    std::string note = freqToNoteName(261.63);
    TEST("C4 note name", note == "C4");
}

void testNoteNameMiddleC() {
    std::string note = freqToNoteName(261.63);
    TEST("Middle C", note == "C4");
}

void testNoteNameG3() {
    std::string note = freqToNoteName(196.0);
    TEST("G3 note name", note == "G3");
}

void testNoteNameC5() {
    std::string note = freqToNoteName(523.25);
    TEST("C5 note name", note == "C5");
}

void testNoteNameSharp() {
    std::string note = freqToNoteName(466.16);
    TEST("C#4 note name", note == "C#4");
}

void testHammingWindow() {
    auto window = generateHammingWindow(100);
    TEST("Hamming window size", window.size() == 100);
    TEST("Hamming window first value approx 0.08", window[0] > 0.07 && window[0] < 0.09);
    TEST("Hamming window midpoint ~0.54", window[49] > 0.53 && window[49] < 0.55);
}

void testFixedBufferBasic() {
    FixedBuffer buf(10);
    TEST("FixedBuffer initial empty", !buf.isFull());

    for (int i = 0; i < 10; ++i) {
        buf.put(static_cast<double>(i));
    }

    TEST("FixedBuffer isFull after 10 puts", buf.isFull());

    auto data = buf.getBuffer();
    TEST("FixedBuffer getBuffer returns 10 elements", data.size() == 10);
}

void testFixedBufferOverflow() {
    FixedBuffer buf(5);
    for (int i = 0; i < 7; ++i) {
        buf.put(static_cast<double>(i));
    }

    TEST("FixedBuffer overflow protection", buf.isFull());
    auto data = buf.getBuffer();
    TEST("FixedBuffer holds exactly 5 elements", data.size() == 5);
}

void testLowPassFilter() {
    LowPassFilter lpf(1000.0, 48000.0);
    double output = lpf.process(1.0);
    TEST("LowPassFilter initial output", output >= 0.0 && output <= 1.0);

    double steadyState = 0.0;
    for (int i = 0; i < 100; ++i) {
        steadyState = lpf.process(1.0);
    }
    TEST("LowPassFilter reaches steady state", fabs(steadyState - 1.0) < 0.01);
}

int main() {
    std::cout << "=== NoteReader Test Suite ===\n\n";

    std::cout << "Frequency Detection Tests:\n";
    testFrequencyDetection440Hz();
    testFrequencyDetection523Hz();
    testFrequencyDetection659Hz();
    testFrequencyDetectionLow();

    std::cout << "\nNote Name Tests:\n";
    testNoteNameA4();
    testNoteNameC4();
    testNoteNameMiddleC();
    testNoteNameG3();
    testNoteNameC5();
    testNoteNameSharp();

    std::cout << "\nHamming Window Tests:\n";
    testHammingWindow();

    std::cout << "\nFixedBuffer Tests:\n";
    testFixedBufferBasic();
    testFixedBufferOverflow();

    std::cout << "\nLowPassFilter Tests:\n";
    testLowPassFilter();

    std::cout << "\n=== Test Results ===\n";
    std::cout << "Passed: " << testsPassed << "\n";
    std::cout << "Failed: " << testsFailed << "\n";

    return testsFailed > 0 ? 1 : 0;
}
