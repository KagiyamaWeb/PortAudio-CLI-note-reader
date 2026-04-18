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

std::vector<double> generateHarmonicSignal(double fundFreq, double sampleRate, size_t count,
                                     double fundAmp, double h2Amp, double h3Amp, double h4Amp,
                                     double noiseAmp = 0.0) {
    std::vector<double> signal(count);
    for (size_t i = 0; i < count; ++i) {
        double t = static_cast<double>(i) / sampleRate;
        double sample = fundAmp * sin(2 * M_PI * fundFreq * t);
        
        if (h2Amp > 0) sample += h2Amp * sin(2 * M_PI * fundFreq * 2.0 * t);
        if (h3Amp > 0) sample += h3Amp * sin(2 * M_PI * fundFreq * 3.0 * t);
        if (h4Amp > 0) sample += h4Amp * sin(2 * M_PI * fundFreq * 4.0 * t);
        
        if (noiseAmp > 0) {
            sample += noiseAmp * (static_cast<double>(rand()) / RAND_MAX * 2.0 - 1.0);
        }
        
        signal[i] = sample;
    }
    return signal;
}

void testHarmonicSignal440Stronger2nd() {
    constexpr double FUND_FREQ = 440.0;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t FFT_SIZE = 4096;
    
    std::vector<double> signal = generateHarmonicSignal(
        FUND_FREQ, SAMPLE_RATE, FFT_SIZE,
        0.3,   // f0 amplitude (weak)
        0.8,   // 2nd harmonic (stronger!)
        0.5,   // 3rd harmonic
        0.0,   // 4th harmonic
        0.0    // noise
    );
    
    double fftDetected = runFFTDetection(signal.data(), signal.size(), FFT_SIZE, SAMPLE_RATE);
    double autoDetected = runAutocorrelationPitch(signal.data(), signal.size(), SAMPLE_RATE, 20.0, 4000.0);
    double yinDetected = runYinPitch(signal.data(), signal.size(), SAMPLE_RATE, 0.15);
    
    std::ostringstream msg;
    msg << "Harmonic 440Hz: FFT=" << fftDetected << "Hz, Auto=" << autoDetected 
        << "Hz, YIN=" << yinDetected << "Hz";
    
    bool fftWrong = fftDetected > 800 && fftDetected < 900;
    bool f0Correct = (autoDetected > 420 && autoDetected < 460) || 
                     (yinDetected > 420 && yinDetected < 460);
    
    TEST(msg.str(), fftWrong && f0Correct);
}

void testHarmonicSignal440Stronger3rd() {
    constexpr double FUND_FREQ = 440.0;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t FFT_SIZE = 4096;
    
    std::vector<double> signal = generateHarmonicSignal(
        FUND_FREQ, SAMPLE_RATE, FFT_SIZE,
        0.2,   // f0 amplitude (weakest)
        0.3,   // 2nd harmonic
        0.9,   // 3rd harmonic (strongest!)
        0.4,   // 4th harmonic
        0.0
    );
    
    double fftDetected = runFFTDetection(signal.data(), signal.size(), FFT_SIZE, SAMPLE_RATE);
    double autoDetected = runAutocorrelationPitch(signal.data(), signal.size(), SAMPLE_RATE, 20.0, 4000.0);
    double yinDetected = runYinPitch(signal.data(), signal.size(), SAMPLE_RATE, 0.15);
    
    std::ostringstream msg;
    msg << "Harmonic 440Hz 3rd: FFT=" << fftDetected << "Hz, Auto=" << autoDetected 
        << "Hz, YIN=" << yinDetected << "Hz";
    
    bool fftWrong = fftDetected > 1200 && fftDetected < 1400;
    bool f0Correct = (autoDetected > 420 && autoDetected < 460) || 
                     (yinDetected > 420 && yinDetected < 460);
    
    TEST(msg.str(), fftWrong && f0Correct);
}

void testHarmonicSignalC4Stronger2nd() {
    constexpr double FUND_FREQ = 261.63;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t FFT_SIZE = 4096;
    
    std::vector<double> signal = generateHarmonicSignal(
        FUND_FREQ, SAMPLE_RATE, FFT_SIZE,
        0.25,  // f0 (weak)
        1.0,   // 2nd harmonic (strongest)
        0.6,   // 3rd
        0.4,   // 4th
        0.0
    );
    
    double fftDetected = runFFTDetection(signal.data(), signal.size(), FFT_SIZE, SAMPLE_RATE);
    double autoDetected = runAutocorrelationPitch(signal.data(), signal.size(), SAMPLE_RATE, 20.0, 4000.0);
    double yinDetected = runYinPitch(signal.data(), signal.size(), SAMPLE_RATE, 0.15);
    
    std::ostringstream msg;
    msg << "Harmonic C4: FFT=" << fftDetected << "Hz, Auto=" << autoDetected 
        << "Hz, YIN=" << yinDetected << "Hz";
    
    bool fftWrong = fftDetected > 500 && fftDetected < 530;
    bool f0Correct = (autoDetected > 250 && autoDetected < 275) || 
                     (yinDetected > 250 && yinDetected < 275);
    
    TEST(msg.str(), fftWrong && f0Correct);
}

void testMixedBackground440() {
    constexpr double FUND_FREQ = 440.0;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t FFT_SIZE = 4096;
    
    std::vector<double> signal = generateHarmonicSignal(
        FUND_FREQ, SAMPLE_RATE, FFT_SIZE,
        0.15,  // f0 (very weak)
        0.4,   // 2nd harmonic
        0.7,   // 3rd (strong)
        0.3,   // 4th
        0.05   // add some noise
    );
    
    double fftDetected = runFFTDetection(signal.data(), signal.size(), FFT_SIZE, SAMPLE_RATE);
    double autoDetected = runAutocorrelationPitch(signal.data(), signal.size(), SAMPLE_RATE, 20.0, 4000.0);
    double yinDetected = runYinPitch(signal.data(), signal.size(), SAMPLE_RATE, 0.15);
    
    std::ostringstream msg;
    msg << "Mixed 440Hz: FFT=" << fftDetected << "Hz, Auto=" << autoDetected 
        << "Hz, YIN=" << yinDetected << "Hz";
    
    bool f0Correct = (fabs(fftDetected - 440.0) < 1.0) ||
                    (autoDetected > 420 && autoDetected < 460) || 
                    (yinDetected > 420 && yinDetected < 460);
    
    TEST(msg.str(), f0Correct);
}

void testMixedBackgroundC4() {
    constexpr double FUND_FREQ = 261.63;
    constexpr double SAMPLE_RATE = 48000.0;
    constexpr size_t FFT_SIZE = 4096;
    
    std::vector<double> signal = generateHarmonicSignal(
        FUND_FREQ, SAMPLE_RATE, FFT_SIZE,
        0.1,   // f0 very weak
        0.25,   // 2nd
        0.5,    // 3rd strongest
        0.35,   // 4th
        0.08    // noise
    );
    
    double fftDetected = runFFTDetection(signal.data(), signal.size(), FFT_SIZE, SAMPLE_RATE);
    double autoDetected = runAutocorrelationPitch(signal.data(), signal.size(), SAMPLE_RATE, 20.0, 4000.0);
    double yinDetected = runYinPitch(signal.data(), signal.size(), SAMPLE_RATE, 0.15);
    
    std::ostringstream msg;
    msg << "Mixed C4: FFT=" << fftDetected << "Hz, Auto=" << autoDetected 
        << "Hz, YIN=" << yinDetected << "Hz";
    
    bool fftFound3rd = fftDetected > 760 && fftDetected < 800;
    bool f0Correct = (autoDetected > 250 && autoDetected < 275) || 
                     (yinDetected > 250 && yinDetected < 275);
    
    TEST(msg.str(), fftFound3rd || f0Correct);
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

    std::cout << "\n=== Harmonics Tests (FFT vs f0 detection) ===\n";
    testHarmonicSignal440Stronger2nd();
    testHarmonicSignal440Stronger3rd();
    testHarmonicSignalC4Stronger2nd();
    testMixedBackground440();
    testMixedBackgroundC4();

    std::cout << "\n=== Test Results ===\n";
    std::cout << "Passed: " << testsPassed << "\n";
    std::cout << "Failed: " << testsFailed << "\n";

    return testsFailed > 0 ? 1 : 0;
}
