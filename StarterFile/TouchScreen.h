#ifdef __cplusplus
extern "C" {
#endif

#ifndef TouchScreen_H_
#define TouchScreen_H_

#include <stdlib.h>
#include <stdbool.h>


typedef struct TouchScreenTaskData{
    int* touchState;        //0 = measurement, 1 = battery, 2 = Alarm Battery screen is separate
    int* HVILState;         //0 = s1, 1 = s2, 2 = s3
    int* OvercurrentState;  //0 = s1, 1 = s2, 2 = s3
    int* HVOutOfRangeState; //0 = s1, 1 = s2, 2 = s3
    bool* contactorState; //true = open, false = closed
    bool* batteryOn;
    float* temperature;
  	float* hvCurrent;
    float* hvVoltage;
    int* SOCreading;
    bool* HVIL;
    bool* initialize; //tells us if we need to inialize the function
    bool* nScreen;
    bool* csState;
    bool* finishedFlag //says if function has run all the way through
}touchScreenData;

void touchScreenTask (void*);

#endif

#ifdef __cplusplus
}
#endif
