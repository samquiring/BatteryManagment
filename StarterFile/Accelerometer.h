#ifdef __cplusplus
extern "C" {
#endif

#ifndef Accelerometer_H_
#define Accelerometer_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>

#define BUFFER_SIZE 10


typedef struct accelerometerTaskData{

    int* xRawAcc;
    int* yRawAcc;
    int* zRawAcc;

    int* xOffset;
    int* yOffset;
    int* zOffset;

    float* xAcc;
    float* yAcc;
    float* zAcc;

    float* xAccBuff;
    float* yAccBuff;
    float* zAccBuff;

    float** xBuffer;
    float** yBuffer;
    float** zBuffer;

    int* xPtr;
    int* yPtr;
    int* zPtr;

    bool* xBufferFull;
    bool* yBufferFull;
    bool* zBufferFull;

    int* bufferSize;

    float* xVel;
    float* yVel;
    float* zVel;

    float* xDisplacement;
    float* yDisplacement;
    float* zDisplacement;

    float* totalDistance;
    float* timeBase;
    float* xAng;
    float* yAng;
    float* zAng;

    const byte* xPin;
    const byte* yPin;
    const byte* zPin;

    volatile bool* accelerometerFlag;
    
}accelerometerData;

void accelerometerTask (void*);

#endif

#ifdef __cplusplus
}
#endif
