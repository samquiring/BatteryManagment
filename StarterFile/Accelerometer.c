
#include <stdlib.h>
#include <stdbool.h>
#include "Accelerometer.h"
#include <Arduino.h>
#define zeroPoint 0
#define movementErrorMargin 100
#define gEquivalent

float tempX = 0;
float tempY = 0;
float tempZ = 0;
// converts the x, y, and z accelerations into cm / s^2

    void convertFromRaw(int* xRawAcc, int* yRawAcc, int* zRawAcc, float* xAcc, float* yAcc, float* zAcc) {

        float xGAcc = ((*xRawAcc - zeroPoint) / 160.0);
        float yGAcc = ((*yRawAcc - zeroPoint) / 87.0);
        float zGAcc = ((*zRawAcc - zeroPoint) / 160.0);

        *xAcc = xGAcc * 980.0;
        *yAcc = yGAcc * 980.0;
        *zAcc = zGAcc * 980.0;

    }

    void updateVelocity(float* xAcc, float* yAcc, float* zAcc, float* xVel, float* yVel, float* zVel, float* timeBase) {
       *xVel = *xVel + *xAcc * *timeBase;
       *yVel = *yVel + *yAcc * *timeBase;
       *zVel = *zVel + *zAcc * *timeBase;
      
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
      
      if(*xAcc > 980.0){
        tempX = 980.0;
      } else if(*xAcc < -980.0){
        tempX = -980.0;
      } else {
        tempX = *xAcc;
      }

      if(*yAcc > 980.0){
        tempY = 980.0;
      } else if(*yAcc < -980.0){
        tempY = -980.0;
      } else {
        tempY = *yAcc;
      }

      if(*zAcc > 980.0){
        tempZ = 980.0;
      } else if(*zAcc < -980.0){
        tempZ = -980.0;
      } else {
        tempZ = *zAcc;
      }
      *xAng = acos(tempX/980.0) * 360.0 / 6.28;
      *yAng = acos(tempY/980.0) * 360.0 / 6.28;
      *zAng = acos(tempZ/980.0) * 360.0 / 6.28;
    }  

    void getPinData(int* xRaw, int* yRaw, int* zRaw, const byte* xPin, const byte* yPin, const byte* zPin, int* xOffset, int* yOffset, int* zOffset){

      *xRaw = analogRead(*xPin) - *xOffset;
      *yRaw = analogRead(*yPin) - *yOffset;
      *zRaw = analogRead(*zPin) - *zOffset;
    }
    
    void updateBuffer(float* Buffer, float* Raw, int* bufferPtr, float* denoiced, float* bufferFull, int* bufferSize){
      *denoiced += ((*Raw) - Buffer[*bufferPtr])/(*bufferSize);
      Buffer[*bufferPtr] = (*Raw);
      if(*bufferPtr != *bufferSize){
        *bufferPtr += 1;
      } else {
        *bufferFull = true;
        *bufferPtr = 0;
      }
    }


    void accelerometerTask(void *mData) {

        accelerometerData* data = (accelerometerData*) mData;

        if(*(data->accelerometerFlag)){
            getPinData(data->xRawAcc, data->yRawAcc, data->zRawAcc, data->xPin, data->yPin, data->zPin, data->xOffset, data->yOffset, data->zOffset);
            convertFromRaw(data->xRawAcc, data->yRawAcc, data->zRawAcc, data->xAcc, data->yAcc, data->zAcc);
            //updateBuffer(data->xBuffer, data->xAcc, data->xPtr, data->xAccBuff, data->xBufferFull, data->bufferSize);
            //updateBuffer(data->yBuffer, data->yAcc, data->yPtr, data->yAccBuff, data->yBufferFull, data->bufferSize);
            //updateBuffer(data->zBuffer, data->zAcc, data->zPtr, data->zAccBuff, data->zBufferFull, data->bufferSize);
            //updateBuffer(data->bigXBuffer, data->xAcc, data->bigPtrX, data->bigX, data->xBufferFull, data->bigBufferSize);
            //updateBuffer(data->bigYBuffer, data->yAcc, data->bigPtrY, data->bigY, data->yBufferFull, data->bigBufferSize);
            //updateDisplacement(data->xDisplacement, data->xAccBuff, data->xVel, data->yDisplacement, data->yAccBuff, data->yVel, data->zDisplacement, data->zAccBuff, data->zVel, data->timeBase);
            //updateDistance(data->totalDistance, data->xAccBuff, data->xVel, data->yAccBuff, data->yVel, data->zAccBuff, data->zVel, data->timeBase);
            updateAngles(data->xAcc, data->yAcc, data->zAcc, data->xAng, data->yAng, data->zAng);
        }
        *(data->accelerometerFlag) = true;
    }
