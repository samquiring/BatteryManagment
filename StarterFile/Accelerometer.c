
#include <stdlib.h>
#include <stdbool.h>
#include "Accelerometer.h"
#include <Arduino.h>
#define xOffset 0
#define yOffset 0
#define zOffset 0
#define zeroPoint 300
#define movementErrorMargin 0
#define gEquivalent

// converts the x, y, and z accelerations into cm / s^2

    void convertFromRaw(int* xRawAcc, int* yRawAcc, int* zRawAcc, float* xAcc, float* yAcc, float* zAcc) {

        float xGAcc = ((*xRawAcc - zeroPoint) / 160.0);
        float yGAcc = ((*yRawAcc - zeroPoint) / 160.0);
        float zGAcc = ((*zRawAcc - zeroPoint) / 160.0);

        *xAcc = xGAcc * 980.0;
        *yAcc = yGAcc * 980.0;
        *zAcc = zGAcc * 980.0;

    }

    void updateDisplacement(float* xDisplacement, float* xAcc, float* xVel, float* yDisplacement, float* yAcc, float* yVel, float* zDisplacement, float* zAcc, float* zVel, float* timeBase) {


        if ((*zAcc < 980.0 - movementErrorMargin) || (*zAcc > 980.0 + movementErrorMargin)) {
            *xDisplacement = *xDisplacement + (*xVel * *timeBase) + 0.5 * *xAcc * (*timeBase * *timeBase);
            *yDisplacement = *yDisplacement + (*yVel * *timeBase) + 0.5 * *yAcc * (*timeBase * *timeBase);
            *zDisplacement = *zDisplacement + (*zVel * *timeBase) + 0.5 * (*zAcc - 980.0) * (*timeBase * *timeBase);
        }
    }

    void updateDistance(float* totalDistance, float* xAcc, float* xVel, float* yAcc, float* yVel, float* zAcc, float* zVel, float* timeBase){

        if ((*zAcc < 980.0 - movementErrorMargin) || (*zAcc > 980.0 + movementErrorMargin)) {
            float xChange = (*xVel * *timeBase) + 0.5 * *xAcc * (*timeBase * *timeBase);
            float yChange = (*yVel * *timeBase) + 0.5 * *yAcc * (*timeBase * *timeBase);
            float zChange = (*zVel * *timeBase) + 0.5 * (*zAcc - 980.0) * (*timeBase * *timeBase);


            float distanceChange = sqrt((xChange * xChange) + (yChange * yChange) + (zChange * zChange));

            *totalDistance = *totalDistance + distanceChange;
        }

    }

    void updateAngles (float* xAcc, float* yAcc, float* zAcc, float* xAng, float* yAng, float* zAng) {

        if ((*zAcc > 980.0 - movementErrorMargin) || (*zAcc < 980.0 + movementErrorMargin)) {
        
          *xAng = acos(*yAcc);
          *yAng = acos(*xAcc);
          *zAng = acos(*zAcc);
        }
    }

    void getPinData(int* xRaw, int* yRaw, int* zRaw, const byte* xPin, const byte* yPin, const byte* zPin){

      *xRaw = analogRead(*xPin);
      *yRaw = analogRead(*yPin);
      *zRaw = analogRead(*zPin);
    }


    void accelerometerTask(void *mData) {

        accelerometerData* data = (accelerometerData*) mData;

        if(*(data->accelerometerFlag)){
            getPinData(data->xRawAcc, data->yRawAcc, data->zRawAcc, data->xPin, data->yPin, data->zPin);
            convertFromRaw(data->xRawAcc, data->yRawAcc, data->zRawAcc, data->xAcc, data->yAcc, data->zAcc);
            updateDisplacement(data->xDisplacement, data->xAcc, data->xVel, data->yDisplacement, data->yAcc, data->yVel, data->zDisplacement, data->zAcc, data->zVel, data->timeBase);
            updateDistance(data->totalDistance, data->xAcc, data->xVel, data->yAcc, data->yVel, data->zAcc, data->zVel, data->timeBase);
            updateAngles(data->xAcc, data->yAcc, data->zAcc, data->xAng, data->yAng, data->zAng);

        }
    }
