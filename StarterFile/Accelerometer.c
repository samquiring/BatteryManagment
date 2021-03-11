
#include <stdlib.h>
#include <stdbool.h>
#include "Accelerometer.h"
#include <Arduino.h>
#define xOffset 0;
#define yOffset 0;
#define zOffset 0;

    void updateDisplacement(float* xDisplacement, float* xAcc, float* xVel, float* yDisplacement, float* yAcc, float* yVel, float* zDisplacement, float* zAcc, float* zVel, float* timeBase) {

        *xDisplacement = *xDisplacement + (*xVel * *timeBase) + 0.5 * xAcc * (*timeBase * *timeBase);
        *yDisplacement = *yDisplacement + (*yVel * *timeBase) + 0.5 * yAcc * (*timeBase * *timeBase);

            // probably gonna have to account for the z acceleration.
        *zDisplacement = *zDisplacement + (*zVel * *timeBase) + 0.5 * (zAcc - 9.8) * (*timeBase * *timeBase);
    }

    void updateDistance(float* totalDistance, float* xAcc, float* xVel, float* yAcc, float* yVel, float* Acc, float *xVel, float* timeBase){

        float xChange = (*xVel * *timeBase) + 0.5 * xAcc * (*timeBase * *timeBase);
        float yChange = (*yVel * *timeBase) + 0.5 * yAcc * (*timeBase * *timeBase);
        float zChange = (*zVel * *timeBase) + 0.5 * (zAcc - 9.8) * (*timeBase * *timeBase);

        float distanceChange = sqrt((xChange * xChange) + (yChange * yChange)+ (zChange * zChange));

        *totalDistance = *totalDistance + distanceChange;

    }


