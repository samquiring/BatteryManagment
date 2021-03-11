#ifdef __cplusplus
extern "C" {
#endif

#ifndef Accelerometer_H_
#define Accelerometer_H_

#include <stdlib.h>
#include <stdbool.h>



typedef struct AccelerometerTaskData{

    float* xAcc;
    float* yAcc;
    float* zAcc;

    float* xVel;
    float* yVel;
    float* zVel;

    float* xDistance;
    float* xDisplacement;
    float* yDistance;
    float* yDisplacement;
    float* zDistance;
    float* zDisplacement;

    float* totalDistance;

}AccelerometerData;

void AccelerometerTask (void*);

#endif

#ifdef __cplusplus
}
#endif
