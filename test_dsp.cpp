#include "dsp.h"
#include "note_detector.h"
#include "audio_utils.h"

#include <cmath>
#include <iostream>
#include <vector>
#include <cassert>
#include <sstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

void testFrequencyDetection440Hz() {
    constexpr double TARGET_FREQ = 440.0;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t FFT_SIZE = 4096;

    std::vector<double> signal(FFT_SIZE);
    for (size_t i = 0; i < FFT_SIZE; ++i) {
        signal[i] = sin(2 * M_PI * TARGET_FREQ * i / SAMPLE_RATE);
    }

    double detected = runFFTDetection(signal.data(), signal.size(), FFT_SIZE, SAMPLE_RATE);
    double error = fabs(detected - TARGET_FREQ);

    std::ostringstream msg;
    msg << "440Hz detection (error: " << error << "Hz)";
    msg << " (detected: " << detected << "Hz)";
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

    double detected = runFFTDetection(signal.data(), signal.size(), FFT_SIZE, SAMPLE_RATE);
    double error = fabs(detected - TARGET_FREQ);

    std::ostringstream msg;
    msg << "523Hz detection (error: " << error << "Hz)";
    msg << " (detected: " << detected << "Hz)";
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

    double detected = runFFTDetection(signal.data(), signal.size(), FFT_SIZE, SAMPLE_RATE);
    double error = fabs(detected - TARGET_FREQ);

    std::ostringstream msg;
    msg << "659Hz detection (error: " << error << "Hz)";
    msg << " (detected: " << detected << "Hz)";
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

    double detected = runFFTDetection(signal.data(), signal.size(), FFT_SIZE, SAMPLE_RATE);
    double error = fabs(detected - TARGET_FREQ);

    std::ostringstream msg;
    msg << "100Hz detection (error: " << error << "Hz)";
    msg << " (detected: " << detected << "Hz)";
    TEST(msg.str(), error < 1.0);
}

void testNoteDetection440Hz() {
    constexpr double TARGET_FREQ = 440.0;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t FFT_SIZE = 4096;

    std::vector<double> signal(FFT_SIZE);
    for (size_t i = 0; i < FFT_SIZE; ++i) {
        signal[i] = sin(2 * M_PI * TARGET_FREQ * i / SAMPLE_RATE);
    }

    double detected = runFFTDetection(signal.data(), signal.size(), FFT_SIZE, SAMPLE_RATE);
    std::string note = freqToNoteName(detected);

    TEST("440Hz/A4 detection", note == "A4");
}

void testNoteDetection260Hz() {
    constexpr double TARGET_FREQ = 260.0;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t FFT_SIZE = 4096;

    std::vector<double> signal(FFT_SIZE);
    for (size_t i = 0; i < FFT_SIZE; ++i) {
        signal[i] = sin(2 * M_PI * TARGET_FREQ * i / SAMPLE_RATE);
    }

    double detected = runFFTDetection(signal.data(), signal.size(), FFT_SIZE, SAMPLE_RATE);
    std::string note = freqToNoteName(detected);

    TEST("261Hz/C4 detection", note == "C4");
}

void testNoteDetection198Hz() {
    constexpr double TARGET_FREQ = 198.0;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t FFT_SIZE = 4096;

    std::vector<double> signal(FFT_SIZE);
    for (size_t i = 0; i < FFT_SIZE; ++i) {
        signal[i] = sin(2 * M_PI * TARGET_FREQ * i / SAMPLE_RATE);
    }

    double detected = runFFTDetection(signal.data(), signal.size(), FFT_SIZE, SAMPLE_RATE);
    std::string note = freqToNoteName(detected);

    TEST("196Hz/G3 detection", note == "G3");
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
    TEST("A#4 note name", note == "A#4");
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

void testAutocorrelation440Hz() {
    constexpr double TARGET_FREQ = 440.0;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t SAMPLE_COUNT = 4096;
    constexpr double MIN_FREQ = 20.0;
    constexpr double MAX_FREQ = 4000.0;

    std::vector<double> signal(SAMPLE_COUNT);
    for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
        signal[i] = sin(2 * M_PI * TARGET_FREQ * i / SAMPLE_RATE);
    }

    double detected = runAutocorrelationPitch(signal.data(), signal.size(), SAMPLE_RATE, MIN_FREQ, MAX_FREQ);
    double error = fabs(detected - TARGET_FREQ);

    std::ostringstream msg;
    msg << "Autocorrelation 440Hz (detected: " << detected << "Hz, error: " << error << "Hz)";
    TEST(msg.str(), detected > 0 && error < 2.0);
}

void testAutocorrelation523Hz() {
    constexpr double TARGET_FREQ = 523.25;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t SAMPLE_COUNT = 4096;
    constexpr double MIN_FREQ = 20.0;
    constexpr double MAX_FREQ = 4000.0;

    std::vector<double> signal(SAMPLE_COUNT);
    for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
        signal[i] = sin(2 * M_PI * TARGET_FREQ * i / SAMPLE_RATE);
    }

    double detected = runAutocorrelationPitch(signal.data(), signal.size(), SAMPLE_RATE, MIN_FREQ, MAX_FREQ);
    double error = fabs(detected - TARGET_FREQ);

    std::ostringstream msg;
    msg << "Autocorrelation 523Hz (detected: " << detected << "Hz, error: " << error << "Hz)";
    TEST(msg.str(), detected > 0 && error < 2.0);
}

void testAutocorrelationC4() {
    constexpr double TARGET_FREQ = 261.63;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t SAMPLE_COUNT = 4096;
    constexpr double MIN_FREQ = 20.0;
    constexpr double MAX_FREQ = 4000.0;

    std::vector<double> signal(SAMPLE_COUNT);
    for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
        signal[i] = sin(2 * M_PI * TARGET_FREQ * i / SAMPLE_RATE);
    }

    double detected = runAutocorrelationPitch(signal.data(), signal.size(), SAMPLE_RATE, MIN_FREQ, MAX_FREQ);
    double error = fabs(detected - TARGET_FREQ);

    std::ostringstream msg;
    msg << "Autocorrelation C4 (detected: " << detected << "Hz, error: " << error << "Hz)";
    TEST(msg.str(), detected > 0 && error < 2.0);
}

void testAutocorrelationG3() {
    constexpr double TARGET_FREQ = 196.0;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t SAMPLE_COUNT = 4096;
    constexpr double MIN_FREQ = 20.0;
    constexpr double MAX_FREQ = 4000.0;

    std::vector<double> signal(SAMPLE_COUNT);
    for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
        signal[i] = sin(2 * M_PI * TARGET_FREQ * i / SAMPLE_RATE);
    }

    double detected = runAutocorrelationPitch(signal.data(), signal.size(), SAMPLE_RATE, MIN_FREQ, MAX_FREQ);
    double error = fabs(detected - TARGET_FREQ);

    std::ostringstream msg;
    msg << "Autocorrelation G3 (detected: " << detected << "Hz, error: " << error << "Hz)";
    TEST(msg.str(), detected > 0 && error < 2.0);
}

void testYin440Hz() {
    constexpr double TARGET_FREQ = 440.0;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t SAMPLE_COUNT = 4096;
    constexpr double THRESHOLD = 0.15;

    std::vector<double> signal(SAMPLE_COUNT);
    for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
        signal[i] = sin(2 * M_PI * TARGET_FREQ * i / SAMPLE_RATE);
    }

    double detected = runYinPitch(signal.data(), signal.size(), SAMPLE_RATE, THRESHOLD);
    double error = fabs(detected - TARGET_FREQ);

    std::ostringstream msg;
    msg << "YIN 440Hz (detected: " << detected << "Hz, error: " << error << "Hz)";
    TEST(msg.str(), detected > 0 && error < 2.0);
}

void testYin523Hz() {
    constexpr double TARGET_FREQ = 523.25;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t SAMPLE_COUNT = 4096;
    constexpr double THRESHOLD = 0.15;

    std::vector<double> signal(SAMPLE_COUNT);
    for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
        signal[i] = sin(2 * M_PI * TARGET_FREQ * i / SAMPLE_RATE);
    }

    double detected = runYinPitch(signal.data(), signal.size(), SAMPLE_RATE, THRESHOLD);
    double error = fabs(detected - TARGET_FREQ);

    std::ostringstream msg;
    msg << "YIN 523Hz (detected: " << detected << "Hz, error: " << error << "Hz)";
    TEST(msg.str(), detected > 0 && error < 2.0);
}

void testYinC4() {
    constexpr double TARGET_FREQ = 261.63;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t SAMPLE_COUNT = 4096;
    constexpr double THRESHOLD = 0.15;

    std::vector<double> signal(SAMPLE_COUNT);
    for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
        signal[i] = sin(2 * M_PI * TARGET_FREQ * i / SAMPLE_RATE);
    }

    double detected = runYinPitch(signal.data(), signal.size(), SAMPLE_RATE, THRESHOLD);
    double error = fabs(detected - TARGET_FREQ);

    std::ostringstream msg;
    msg << "YIN C4 (detected: " << detected << "Hz, error: " << error << "Hz)";
    TEST(msg.str(), detected > 0 && error < 2.0);
}

void testYinG3() {
    constexpr double TARGET_FREQ = 196.0;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t SAMPLE_COUNT = 4096;
    constexpr double THRESHOLD = 0.15;

    std::vector<double> signal(SAMPLE_COUNT);
    for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
        signal[i] = sin(2 * M_PI * TARGET_FREQ * i / SAMPLE_RATE);
    }

    double detected = runYinPitch(signal.data(), signal.size(), SAMPLE_RATE, THRESHOLD);
    double error = fabs(detected - TARGET_FREQ);

    std::ostringstream msg;
    msg << "YIN G3 (detected: " << detected << "Hz, error: " << error << "Hz)";
    TEST(msg.str(), detected > 0 && error < 2.0);
}

void testCepstrum440Hz() {
    constexpr double TARGET_FREQ = 440.0;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t FFT_SIZE = 4096;
    constexpr size_t SAMPLE_COUNT = 4096;
    constexpr double MIN_FREQ = 20.0;
    constexpr double MAX_FREQ = 4000.0;

    std::vector<double> signal(SAMPLE_COUNT);
    for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
        signal[i] = sin(2 * M_PI * TARGET_FREQ * i / SAMPLE_RATE);
    }

    double detected = runCepstrumPitch(signal.data(), signal.size(), FFT_SIZE, SAMPLE_RATE, MIN_FREQ, MAX_FREQ);
    double error = fabs(detected - TARGET_FREQ);

    std::ostringstream msg;
    msg << "Cepstrum 440Hz (detected: " << detected << "Hz, error: " << error << "Hz)";
    TEST(msg.str(), detected > 0 && error < 5.0);
}

void testCepstrum523Hz() {
    constexpr double TARGET_FREQ = 523.25;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t FFT_SIZE = 4096;
    constexpr size_t SAMPLE_COUNT = 4096;
    constexpr double MIN_FREQ = 20.0;
    constexpr double MAX_FREQ = 4000.0;

    std::vector<double> signal(SAMPLE_COUNT);
    for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
        signal[i] = sin(2 * M_PI * TARGET_FREQ * i / SAMPLE_RATE);
    }

    double detected = runCepstrumPitch(signal.data(), signal.size(), FFT_SIZE, SAMPLE_RATE, MIN_FREQ, MAX_FREQ);
    double error = fabs(detected - TARGET_FREQ);

    std::ostringstream msg;
    msg << "Cepstrum 523Hz (detected: " << detected << "Hz, error: " << error << "Hz)";
    TEST(msg.str(), detected > 0 && error < 5.0);
}

void testCepstrumC4() {
    constexpr double TARGET_FREQ = 261.63;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t FFT_SIZE = 4096;
    constexpr size_t SAMPLE_COUNT = 4096;
    constexpr double MIN_FREQ = 20.0;
    constexpr double MAX_FREQ = 4000.0;

    std::vector<double> signal(SAMPLE_COUNT);
    for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
        signal[i] = sin(2 * M_PI * TARGET_FREQ * i / SAMPLE_RATE);
    }

    double detected = runCepstrumPitch(signal.data(), signal.size(), FFT_SIZE, SAMPLE_RATE, MIN_FREQ, MAX_FREQ);
    double error = fabs(detected - TARGET_FREQ);

    std::ostringstream msg;
    msg << "Cepstrum C4 (detected: " << detected << "Hz, error: " << error << "Hz)";
    TEST(msg.str(), detected > 0 && error < 5.0);
}

void testCepstrumG3() {
    constexpr double TARGET_FREQ = 196.0;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t FFT_SIZE = 4096;
    constexpr size_t SAMPLE_COUNT = 4096;
    constexpr double MIN_FREQ = 20.0;
    constexpr double MAX_FREQ = 4000.0;

    std::vector<double> signal(SAMPLE_COUNT);
    for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
        signal[i] = sin(2 * M_PI * TARGET_FREQ * i / SAMPLE_RATE);
    }

    double detected = runCepstrumPitch(signal.data(), signal.size(), FFT_SIZE, SAMPLE_RATE, MIN_FREQ, MAX_FREQ);
    double error = fabs(detected - TARGET_FREQ);

    std::ostringstream msg;
    msg << "Cepstrum G3 (detected: " << detected << "Hz, error: " << error << "Hz)";
    TEST(msg.str(), detected > 0 && error < 5.0);
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

    std::cout << "\nNote Detection Tests:\n";
    testNoteDetection440Hz();
    testNoteDetection198Hz();
    testNoteDetection260Hz();

    std::cout << "\nHamming Window Tests:\n";
    testHammingWindow();

    std::cout << "\nFixedBuffer Tests:\n";
    testFixedBufferBasic();
    testFixedBufferOverflow();

    std::cout << "\nLowPassFilter Tests:\n";
    testLowPassFilter();

    std::cout << "\n=== Pitch Detection Tests (Autocorrelation) ===\n";
    testAutocorrelation440Hz();
    testAutocorrelation523Hz();
    testAutocorrelationC4();
    testAutocorrelationG3();

    std::cout << "\n=== Pitch Detection Tests (YIN) ===\n";
    testYin440Hz();
    testYin523Hz();
    testYinC4();
    testYinG3();

    std::cout << "\n=== Pitch Detection Tests (Cepstrum) ===\n";
    testCepstrum440Hz();
    testCepstrum523Hz();
    testCepstrumC4();
    testCepstrumG3();

    std::cout << "\n=== Test Results ===\n";
    std::cout << "Passed: " << testsPassed << "\n";
    std::cout << "Failed: " << testsFailed << "\n";

    return testsFailed > 0 ? 1 : 0;
}
