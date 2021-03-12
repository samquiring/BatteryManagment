#ifdef __cplusplus
extern "C" {
#endif

#ifndef Accelerometer_H_
#define Accelerometer_H_

#include <stdlib.h>
#include <stdbool.h>



typedef struct AccelerometerTaskData{

    int* xRawAcc;
    int* yRawAcc;
    int* zRawAcc;

    float* xAcc;
    float* yAcc;
    float* zAcc;

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
    
}AccelerometerData;

void AccelerometerTask (void*);

#endif

#ifdef __cplusplus
}
#endif
