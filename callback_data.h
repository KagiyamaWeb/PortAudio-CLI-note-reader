// callback_data.h
#ifndef CALLBACK_DATA_H
#define CALLBACK_DATA_H

#include <fftw3.h>

typedef struct {
    double* fftInput;
    fftw_complex* fftOutput;
    fftw_plan fftPlan;
    unsigned long framesPerBuffer;
} CallbackData ;

#endif // CALLBACK_DATA_H