#include "dsp.h"
#include "audio_utils.h"
#include "note_detector.h"

double runFFTDetection(const double* samples, size_t sampleCount, size_t fftSize, double sampleRate) {
    std::vector<double> windowed(fftSize, 0.0);
    size_t copySize = std::min(sampleCount, fftSize);

    auto hamming = generateHammingWindow(fftSize);
    for (size_t i = 0; i < copySize; ++i) {
        windowed[i] = samples[i] * hamming[i];
    }

    fftw_complex* fftOutput = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fftSize / 2 + 1));
    fftw_plan plan = fftw_plan_dft_r2c_1d(fftSize, windowed.data(), fftOutput, FFTW_ESTIMATE);
    fftw_execute(plan);

    double freq = getFrequency(fftOutput, fftSize, sampleRate);

    fftw_destroy_plan(plan);
    fftw_free(fftOutput);

    return freq;
}
