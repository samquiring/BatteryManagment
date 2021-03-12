
#include <stdlib.h>
#include <stdbool.h>
#include "Accelerometer.h"
#include <Arduino.h>
#define zeroPoint 0
#define movementErrorMargin 100
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

  if (sqrt((*zAcc * *zAcc) + (*yAcc * *yAcc) + (*xAcc * *xAcc)) < 980.0 + movementErrorMargin) {

          if (*zAng > 980.0) {
            *zAng = 90.0;
            *xAng = asin(*yAcc/980);
            *yAng = asin(*xAcc/980);
          } else if (*xAng > 980.0) {
            *xAng = 90.0;
            *yAng = asin(*xAcc/980);
            *zAng = asin(*zAcc/980);
          } else if (*yAng > 980.0) {
            *yAng = 90.0;
            *xAng = asin(*yAcc/980);
            *zAng = asin(*zAcc/980);
          } else {
          *xAng = asin(*yAcc/980) * 360 / 6.28;
          *yAng = asin(*xAcc/980) * 360 / 6.28;
          *zAng = asin(*zAcc/980) * 360 / 6.28;
        }
      }
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
            noInterrupts();
            getPinData(data->xRawAcc, data->yRawAcc, data->zRawAcc, data->xPin, data->yPin, data->zPin, data->xOffset, data->yOffset, data->zOffset);
            convertFromRaw(data->xRawAcc, data->yRawAcc, data->zRawAcc, data->xAcc, data->yAcc, data->zAcc);
            updateBuffer(data->xBuffer, data->xAcc, data->xPtr, data->xAccBuff, data->xBufferFull, data->bufferSize);
            updateBuffer(data->yBuffer, data->yAcc, data->yPtr, data->yAccBuff, data->yBufferFull, data->bufferSize);
            updateBuffer(data->zBuffer, data->zAcc, data->zPtr, data->zAccBuff, data->zBufferFull, data->bufferSize);
            updateBuffer(data->bigXBuffer, data->xAcc, data->bigPtrX, data->bigX, data->xBufferFull, data->bigBufferSize);
            updateBuffer(data->bigYBuffer, data->yAcc, data->bigPtrY, data->bigY, data->yBufferFull, data->bigBufferSize);
            updateDisplacement(data->xDisplacement, data->xAccBuff, data->xVel, data->yDisplacement, data->yAccBuff, data->yVel, data->zDisplacement, data->zAccBuff, data->zVel, data->timeBase);
            updateDistance(data->totalDistance, data->xAccBuff, data->xVel, data->yAccBuff, data->yVel, data->zAccBuff, data->zVel, data->timeBase);
            updateAngles(data->xAcc, data->yAcc, data->zAcc, data->xAng, data->yAng, data->zAng);
            interrupts();
        }
        *(data->accelerometerFlag) = true;
    }
