#ifdef __cplusplus
extern "C" {
#endif

#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_


#define HVIL_OPEN   false
#define HVIL_CLOSED true

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>


typedef struct measurementTaskData {
    bool* hvilStatus;
    const byte* hvilPin;
    const byte* tempPin;
    const byte* currentPin;
    const byte* voltagePin;
    float* temperature;
    float* hvCurrent;
    float* hvVoltage;
    float* currentMin;
    float* currentMax;
    float* voltageMin;
    float* voltageMax;
    float* temperatureMin;
    float* temperatureMax;
    bool* voltageChangeMin;
    bool* voltageChangeMax;
    bool* currentChangeMin;
    bool* currentChangeMax;
    bool* tempChangeMin;
    bool* tempChangeMax;
    int* counter;
    volatile bool* measurementFlag; //true = run task false = skip task
} measurementData;


void measurementTask (void*);


#endif

#ifdef __cplusplus
}
#endif
