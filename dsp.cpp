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

double runAutocorrelationPitch(const double* samples, size_t sampleCount, double sampleRate,
                              double minFreq, double maxFreq) {
    size_t minLag = static_cast<size_t>(sampleRate / maxFreq);
    size_t maxLag = static_cast<size_t>(sampleRate / minFreq);
    
    if (maxLag > sampleCount) maxLag = sampleCount;
    if (minLag < 1) minLag = 1;
    
    double maxCorrelation = 0.0;
    size_t bestLag = 0;
    
    for (size_t lag = minLag; lag <= maxLag; ++lag) {
        double correlation = 0.0;
        double norm1 = 0.0;
        double norm2 = 0.0;
        
        for (size_t i = 0; i < sampleCount - lag; ++i) {
            correlation += samples[i] * samples[i + lag];
            norm1 += samples[i] * samples[i];
            norm2 += samples[i + lag] * samples[i + lag];
        }
        
        double normalizedCorr = correlation / (sqrt(norm1) * sqrt(norm2) + 1e-10);
        
        if (normalizedCorr > maxCorrelation) {
            maxCorrelation = normalizedCorr;
            bestLag = lag;
        }
    }
    
    if (bestLag == 0 || maxCorrelation < 0.5) {
        return 0.0;
    }
    
    double frequency = sampleRate / static_cast<double>(bestLag);
    
    if (frequency < minFreq || frequency > maxFreq) {
        return 0.0;
    }
    
    return frequency;
}

double runYinPitch(const double* samples, size_t sampleCount, double sampleRate,
                   double threshold) {
    size_t halfCount = sampleCount / 2;
    std::vector<double> yinBuffer(sampleCount, 0.0);
    
    yinBuffer[0] = 1.0;
    
    for (size_t tau = 1; tau < halfCount; ++tau) {
        yinBuffer[tau] = 0.0;
        
        for (size_t j = 0; j < halfCount; ++j) {
            double delta = samples[j] - samples[j + tau];
            yinBuffer[tau] += delta * delta;
        }
    }
    
    yinBuffer[0] = 1.0;
    double runningSum = 0.0;
    
    for (size_t tau = 1; tau < halfCount; ++tau) {
        runningSum += yinBuffer[tau];
        yinBuffer[tau] *= tau / runningSum;
    }
    
    size_t tauEstimate = 2;
    while (tauEstimate < halfCount - 1) {
        if (yinBuffer[tauEstimate] < threshold) {
            while (tauEstimate + 1 < halfCount && 
                   yinBuffer[tauEstimate + 1] < yinBuffer[tauEstimate]) {
                ++tauEstimate;
            }
            break;
        }
        ++tauEstimate;
    }
    
    if (tauEstimate == halfCount - 1 || yinBuffer[tauEstimate] >= threshold) {
        return 0.0;
    }
    
    double betterTau = tauEstimate;
    if (tauEstimate > 0 && tauEstimate < halfCount - 1) {
        double s0 = yinBuffer[tauEstimate - 1];
        double s1 = yinBuffer[tauEstimate];
        double s2 = yinBuffer[tauEstimate + 1];
        betterTau = tauEstimate + (s2 - s0) / (2.0 * (2.0 * s1 - s2 - s0));
    }
    
    double frequency = sampleRate / betterTau;
    
    if (frequency < 20.0 || frequency > sampleRate / 2.0) {
        return 0.0;
    }
    
    return frequency;
}

double runCepstrumPitch(const double* samples, size_t sampleCount, size_t fftSize,
                        double sampleRate, double minFreq, double maxFreq) {
    std::vector<double> windowed(fftSize, 0.0);
    size_t copySize = std::min(sampleCount, fftSize);
    
    auto hamming = generateHammingWindow(fftSize);
    for (size_t i = 0; i < copySize; ++i) {
        windowed[i] = samples[i] * hamming[i];
    }
    
    fftw_complex* fftOutput = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fftSize / 2 + 1));
    std::vector<double> realPart(fftSize / 2 + 1);
    
    fftw_plan forwardPlan = fftw_plan_dft_r2c_1d(fftSize, windowed.data(), fftOutput, FFTW_ESTIMATE);
    fftw_execute(forwardPlan);
    
    for (size_t i = 0; i <= fftSize / 2; ++i) {
        double real = fftOutput[i][0];
        double imag = fftOutput[i][1];
        double logMag = log(sqrt(real * real + imag * imag) + 1e-10);
        fftOutput[i][0] = logMag;
        fftOutput[i][1] = 0.0;
    }
    
    std::vector<double> cepstrum(fftSize, 0.0);
    fftw_complex* cepstrumOutput = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize);
    
    fftw_plan inversePlan = fftw_plan_dft_c2r_1d(fftSize, fftOutput, cepstrum.data(), FFTW_ESTIMATE);
    fftw_execute(inversePlan);
    
    for (size_t i = 0; i < fftSize; ++i) {
        cepstrum[i] = cepstrum[i] / fftSize;
    }
    
    size_t minQuefrency = static_cast<size_t>(sampleRate / maxFreq);
    size_t maxQuefrency = static_cast<size_t>(sampleRate / minFreq);
    
    if (maxQuefrency > fftSize / 2) maxQuefrency = fftSize / 2;
    if (minQuefrency < 1) minQuefrency = 1;
    
    double maxCepstrum = 0.0;
    size_t bestQuefrency = 0;
    
    for (size_t q = minQuefrency; q <= maxQuefrency; ++q) {
        if (cepstrum[q] > maxCepstrum) {
            maxCepstrum = cepstrum[q];
            bestQuefrency = q;
        }
    }
    
    fftw_destroy_plan(forwardPlan);
    fftw_destroy_plan(inversePlan);
    fftw_free(fftOutput);
    fftw_free(cepstrumOutput);
    
    if (bestQuefrency == 0) {
        return 0.0;
    }
    
    return sampleRate / static_cast<double>(bestQuefrency);
}
