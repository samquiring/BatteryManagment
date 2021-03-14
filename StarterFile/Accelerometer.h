#ifdef __cplusplus
extern "C" {
#endif

#ifndef Accelerometer_H_
#define Accelerometer_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>


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

    int* xAccBuff;
    int* yAccBuff;
    int* zAccBuff;

    int** xBuffer;
    int** yBuffer;
    int** zBuffer;

    float** bigXBuffer;
    float** bigYBuffer;

    int* xPtr;
    int* yPtr;
    int* zPtr;
    int* bigPtrX;
    int* bigPtrY;

    bool* xBufferFull;
    bool* yBufferFull;
    bool* zBufferFull;
    bool* bigBufferFull;

    int* bufferSize;
    int* bigBufferSize;

    float* bigX;
    float* bigY;

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

    bool* stationary;

    volatile bool* accelerometerFlag;
    
}accelerometerData;

void accelerometerTask (void*);

#endif

#ifdef __cplusplus
}
#endif
