#include <stdlib.h>
#include <portaudio.h>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>

#include "audio_utils.h"
#include "note_detector.h"

static void checkErr(PaError err) {
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main() {
    PaError err = Pa_Initialize();
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

    for (int i = 0; i < numDevices; i++) {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
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
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    PaStream *stream;
    err = Pa_OpenStream(&stream, &inputParameters, nullptr, SAMPLE_RATE, FRAMES_PER_BUFFER, paClipOff, processAudio, nullptr);
    checkErr(err);

    err = Pa_StartStream(stream);
    checkErr(err);

    std::cout << "Stream opened. Press Enter to stop..." << std::endl;
    std::cin.get();

    if (Pa_IsStreamActive(stream) == 1) {
        std::cout << "Stream is active." << std::endl;
    } else {
        std::cerr << "Stream is not active." << std::endl;
    }

    err = Pa_StopStream(stream);
    if (err != paNoError) {
        std::cerr << "Failed to stop audio stream: " << Pa_GetErrorText(err) << std::endl;
    }
    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        std::cerr << "Failed to close audio stream: " << Pa_GetErrorText(err) << std::endl;
    }

    err = Pa_Terminate();
    if (err != paNoError) {
        std::cerr << "Failed to terminate PortAudio: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    return EXIT_SUCCESS;
}