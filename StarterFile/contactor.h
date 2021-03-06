#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONTACTOR_H_
#define CONTACTOR_H_

#include <stdlib.h>
#include <stdbool.h>

typedef struct contactorTaskData{
    bool* contactorState; //true = open, false = closed
    bool* batteryOn;
    volatile bool* hvilAlarm;
    int* overCurrentAlarm;
    int* voltageAlarm;
    volatile bool* contactorFlag; //true = run task false = skip task
}contactorData;

void contactorTask (void*);

#endif

#ifdef __cplusplus
}
#endif
