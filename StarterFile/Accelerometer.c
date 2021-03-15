
#include <stdlib.h>
#include <stdbool.h>
#include "Accelerometer.h"
#include <Arduino.h>
#define zeroPoint 0
#define movementErrorMargin 100
#define gEquivalent
#define gravity 166.0
#define yGravity 87.0
#define offsetError 10.0
#define lowFiltered 5.0

float tempX = 0;
float tempY = 0;
float tempZ = 0;

float xOffsetAcc = 0.0;
float yOffsetAcc = 0.0;
float zOffsetAcc = 0.0;

float prevXAcc = 0.0;
float prevYAcc = 0.0;
float prevZAcc = 0.0;


// converts the x, y, and z accelerations into cm / s^2

    void convertFromRaw(int* xRawAcc, int* yRawAcc, int* zRawAcc, float* xAcc, float* yAcc, float* zAcc, int** xBuffer, int** yBuffer, int* bufferSize, bool* stationary) {

         *xAcc = *xRawAcc * 98.0 / gravity;
         *yAcc = *yRawAcc * 98.0 / yGravity;
         *zAcc = *zRawAcc * 98.0 / gravity;

        *stationary = true;
        for (int i = (*bufferSize-5); i < *bufferSize; i++) {
          int x = *(*xBuffer + i);
          int y = *(*yBuffer + i);
          if (abs(x) > 4 || abs(y) > 4) {
            *stationary = false;
          }
          
        }

    }

    void filterT(float* xAcc, float* yAcc, float* zAcc) {
      if(abs(*xAcc) < lowFiltered) {
        *xAcc = 0.0;
      } else if (abs(*xAcc - prevXAcc) < lowFiltered) {
        *xAcc = prevXAcc;
      } else {
        prevXAcc = *xAcc;
      }    
      if(abs(*yAcc) < lowFiltered) {
        *yAcc = 0.0;
      } else if (abs(*yAcc - prevYAcc) < lowFiltered) {
        *yAcc = prevYAcc;
      } else {
        prevYAcc = *yAcc;
      }   
      if(abs(*zAcc) < lowFiltered) {
        *zAcc = 0.0;
      } else if (abs(*zAcc - prevZAcc) < lowFiltered) {
        *zAcc = prevZAcc;
      } else {
        prevZAcc = *zAcc;
      }     
    }

    void updateVelocity(float* xAcc, float* yAcc, float* zAcc, float* xVel, float* yVel, float* zVel, float* timeBase, bool* bigBufferFull, bool* stationary) {
      //if(*bigBufferFull){  
        *xVel = *xVel + *xAcc * *timeBase;
        *yVel = *yVel + *yAcc * *timeBase;
        *zVel = *zVel + (*zAcc - 980.0) * *timeBase;
     // }
      if(*stationary || (*zAcc < 980.0 - movementErrorMargin) || (*zAcc > 980.0 + movementErrorMargin)){
        *xVel = 0;
        *yVel = 0;
        *zVel = 0;
      }  
    }

    void updateDisplacement(float* xDisplacement, float* xAcc, float* xVel, float* yDisplacement, float* yAcc, float* yVel, float* zDisplacement, float* zAcc, float* zVel, float* timeBase) {

        if ((*zAcc < 980.0 - movementErrorMargin) || (*zAcc > 980.0 + movementErrorMargin)) {
            *xDisplacement = *xDisplacement + (*xVel * *timeBase);
            *yDisplacement = *yDisplacement + (*yVel * *timeBase);
            *zDisplacement =  *zDisplacement + (*zVel * *timeBase);
        }
    }

    void updateDistance(float* totalDistance, float* xAcc, float* xVel, float* yAcc, float* yVel, float* zAcc, float* zVel, float* timeBase){

        if ((*zAcc > 980.0 - movementErrorMargin) || (*zAcc < 980.0 + movementErrorMargin)) {
            float xChange = 10.0 * 0.65 * (*xAcc * *timeBase * *timeBase);
            float yChange = 10.0 * 0.65 * (*yAcc * *timeBase * *timeBase);
            if(*xVel == 0){
              xChange = 0;
              yChange = 0;
            }
            float distanceChange = sqrt((xChange * xChange) + (yChange * yChange));
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

      if (*yAng < 20.0) {
        *yAng = *yAng - 10.0;
      }
   }  

    void getPinData(int* xRaw, int* yRaw, int* zRaw, const byte* xPin, const byte* yPin, const byte* zPin, int* xOffset, int* yOffset, int* zOffset){

      *xRaw = analogRead(*xPin) - *xOffset;
      *yRaw = analogRead(*yPin) - *yOffset;
      *zRaw = analogRead(*zPin) - *zOffset;
    }
    
    void updateBuffer(int** Buffer, int* Raw, int* bufferPtr, int* denoiced, bool* bufferFull, int* bufferSize){
      
      int holder = *(*Buffer+*bufferPtr);
        *denoiced += (*Raw) - holder;
      *(*Buffer+*bufferPtr) = (*Raw);
      if(*bufferPtr != *bufferSize-1){
        *bufferPtr += 1;
      } else {
        *bufferFull = true;
        *bufferPtr = 0;
      }
    }

    void updateOffset(float* bigX, float* bigY, float* xAccBuff, float* yAccBuff, float* zAccBuff){

      if((abs(*bigX + offsetError) >= abs(*xAccBuff)) && (abs(*bigX) - offsetError <= abs(*xAccBuff))){
          if((abs(*bigY + offsetError) >= abs(*yAccBuff)) && (abs(*bigY) - offsetError <= abs(*yAccBuff))){
        *xAccBuff = 0;
        *yAccBuff = 0;
        *zAccBuff = 980;
        *bigX = 0.0;
        *bigY = 0.0;
     }
      }
      
    }


    void accelerometerTask(void *mData) {

        accelerometerData* data = (accelerometerData*) mData;

        if(*(data->accelerometerFlag)){
            getPinData(data->xRawAcc, data->yRawAcc, data->zRawAcc, data->xPin, data->yPin, data->zPin, data->xOffset, data->yOffset, data->zOffset);
            convertFromRaw(data->xAccBuff, data->yAccBuff, data->zAccBuff, data->xAcc, data->yAcc, data->zAcc, data->xBuffer, data->yBuffer, data->bufferSize, data->stationary);
            //updateOffset(data->bigX, data->bigY, data->xAccBuff, data->yAccBuff, data->zAccBuff);
            updateBuffer(data->xBuffer, data->xRawAcc, data->xPtr, data->xAccBuff, data->xBufferFull, data->bufferSize);
            updateBuffer(data->yBuffer, data->yRawAcc, data->yPtr, data->yAccBuff, data->yBufferFull, data->bufferSize);
            updateBuffer(data->zBuffer, data->zRawAcc, data->zPtr, data->zAccBuff, data->zBufferFull, data->bufferSize);
           //updateBuffer(data->bigXBuffer, data->xAcc, data->bigPtrX, data->bigX, data->bigBufferFull, data->bigBufferSize);
            //updateBuffer(data->bigYBuffer, data->yAcc, data->bigPtrY, data->bigY, data->yBufferFull, data->bigBufferSize);
            //filterT(data->xAccBuff, data->yAccBuff, data->zAccBuff);
            updateVelocity(data->xAcc, data->yAcc, data->zAcc, data->xVel, data->yVel, data->zVel, data->timeBase, data->bigBufferFull, data->stationary);
            updateDisplacement(data->xDisplacement, data->xAccBuff, data->xVel, data->yDisplacement, data->yAccBuff, data->yVel, data->zDisplacement, data->zAccBuff, data->zVel, data->timeBase);
            updateDistance(data->totalDistance, data->xAcc, data->xVel, data->yAcc, data->yVel, data->zAcc, data->zVel, data->timeBase);
            updateAngles(data->xAcc, data->yAcc, data->zAcc, data->xAng, data->yAng, data->zAng);
        }
        *(data->accelerometerFlag) = true;
    }
