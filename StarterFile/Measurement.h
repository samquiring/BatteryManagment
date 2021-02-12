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
    int* tempPin;
    int* currentPin;
    int* voltagePin;
    float* temperature;
    float* hvCurrent;
    float* hvVoltage;
    int* counter;
    bool* measurementFlag; //true = run task false = skip task
} measurementData;


void measurementTask (void*);


#endif

#ifdef __cplusplus
}
#endif
