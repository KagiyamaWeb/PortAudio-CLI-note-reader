#include <stdlib.h>
#include <portaudio.h>
#include <iostream>
#include <fftw3.h>
#include <cmath>
#include <string>
//#include "audio_utils.h"
//#include "note_detector.h"
#include "callback_data.h"
#include <vector>

#define SAMPLE_RATE 44100.0   // How many audio samples to capture every second (44100 Hz is standard)
#define FRAMES_PER_BUFFER 512 // How many audio samples to send to our callback function for each channel
#define NUM_CHANNELS 2        // Number of audio channels to capture


void listAudioDevices() {
    int numDevices = Pa_GetDeviceCount();
    if (numDevices < 0) {
        std::cerr << "ERROR: Pa_GetDeviceCount returned " << numDevices << std::endl;
        return;
    }

    const PaDeviceInfo* deviceInfo;
    for (int i = 0; i < numDevices; ++i) {
        deviceInfo = Pa_GetDeviceInfo(i);
        std::cout << "Device #" << i << ": " << deviceInfo->name << std::endl;
    }
}

static void checkErr(PaError err) {
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        exit(EXIT_FAILURE);
    }
}

static CallbackData* data;

double getFrequency(fftw_complex* fftOutput, int numSamples) {
    int maxIndex = 0;
    double maxMagnitude = 0.0;

    for (int i = 0; i < numSamples / 2 + 1; ++i) {
        double magnitude = sqrt(fftOutput[i][0] * fftOutput[i][0] + fftOutput[i][1] * fftOutput[i][1]);
        if (magnitude > maxMagnitude) {
            maxMagnitude = magnitude;
            maxIndex = i;
        }
    }

    double frequency = maxIndex * (double)SAMPLE_RATE / numSamples;
    return frequency;
}

std::string freqToNoteName(double frequency) {
    // Define frequencies for standard A440 tuning
    const char* noteNames[] = {"C", "C/#", "D", "D/#", "E", "F", "F/#", "G", "G/#", "A", "A/#", "B"};
    int numNotes = sizeof(noteNames) / sizeof(noteNames[0]);

    double a4Freq = 440.0;
    int a4Index = 9; // Index of A in noteNames

    int noteIndex = (int)(round(12 * log2(frequency / a4Freq)) + a4Index);
    int octave = noteIndex / numNotes - 1;
    const char* noteName = noteNames[noteIndex % numNotes];

    return std::string(noteName) + std::to_string(octave);
}

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
    if (maxAmplitude < 0.1) {
        return paContinue; // Ignore low-amplitude input
    }

    // Convert frequency to note name
    std::string noteName = freqToNoteName(frequency);

    // Print note name to console
    std::cout << "Detected note: " << noteName << std::endl;

    return paContinue;
}

int main() {
    PaError err;
    err = Pa_Initialize();
    checkErr(err);

    
    int numDevices = Pa_GetDeviceCount();
    printf("Number of devices: %d\n", numDevices);

    if (numDevices < 0) {
        printf("Error getting device count.\n");
        exit(EXIT_FAILURE);
    } else if (numDevices == 0) {
        printf("There are no available audio devices on this machine.\n");
        exit(EXIT_SUCCESS);
    }

    const PaDeviceInfo* deviceInfo;
    for (int i = 0; i < numDevices; i++) {
        deviceInfo = Pa_GetDeviceInfo(i);
        printf("Device %d:\n", i);
        printf("  name: %s\n", deviceInfo->name);
        printf("  maxInputChannels: %d\n", deviceInfo->maxInputChannels);
        printf("  maxOutputChannels: %d\n", deviceInfo->maxOutputChannels);
        printf("  defaultSampleRate: %f\n", deviceInfo->defaultSampleRate);
    }
    int deviceIndex = Pa_GetDefaultInputDevice();
    printf("Default input device: %d\n", deviceIndex);

    PaStreamParameters inputParameters;

    inputParameters.device = Pa_GetDefaultInputDevice();
    inputParameters.channelCount = 1; // Mono input
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    // Allocate memory for FFT input and output arrays
    data = (CallbackData*)malloc(sizeof(CallbackData));
    data->framesPerBuffer = FRAMES_PER_BUFFER;
    data->fftInput = (double*)fftw_malloc(sizeof(double) * FRAMES_PER_BUFFER);
    data->fftOutput = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (FRAMES_PER_BUFFER / 2 + 1));
    data->fftPlan = fftw_plan_dft_r2c_1d(FRAMES_PER_BUFFER, data->fftInput, data->fftOutput, FFTW_ESTIMATE);


    PaStream *stream;
    err = Pa_OpenStream(&stream, &inputParameters, nullptr, SAMPLE_RATE, FRAMES_PER_BUFFER, paClipOff, processAudio, data);
    checkErr(err);

    err = Pa_StartStream(stream);
    checkErr(err);

    std::cout << "Press Enter to stop..." << std::endl;
    std::cin.get();

    // Stop and close the stream
    err = Pa_StopStream(stream);
    if (err != paNoError) {
        std::cerr << "Failed to stop audio stream: " << Pa_GetErrorText(err) << std::endl;
    }
    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        std::cerr << "Failed to close audio stream: " << Pa_GetErrorText(err) << std::endl;
    }

    // Terminate PortAudio
    err = Pa_Terminate();
    if (err != paNoError) {
        std::cerr << "Failed to terminate PortAudio: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    // Clean up FFT resources
    fftw_destroy_plan(data->fftPlan);
    fftw_free(data->fftInput);
    fftw_free(data->fftOutput);

    checkErr(err);

    return EXIT_SUCCESS;
}