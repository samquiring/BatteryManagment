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
    float* SOCreading;
    bool* HVIL;
    bool* initialize; //tells us if we need to inialize the function
    bool* nScreen;
    bool* csState;
    volatile bool* touchScreenFlag; //tells us whether or not to run this task, True = run false = skip
    volatile bool* forceAlarm;      //tells touchScreen to stay in alarm state regardless of user input
    volatile bool* alarmReset;
    bool* diffRate;
    int* counter;
    float* xPosition;
    float* yPosition;
    float* zPosition;
    float* totalDistance;
    int* xAngle;
    int* yAngle;
    int* zAngle;
}touchScreenData;

void touchScreenTask (void*);

#endif

#ifdef __cplusplus
}
#endif
