#ifdef __cplusplus
extern "C" {
#endif

#ifndef Alarm_H_
#define Alarm_H_

#include <stdlib.h>
#include <stdbool.h>


//s1 = "NOT ACTIVE" s2 = "ACTIVE, NOT ACKNOWLEDGED" s3 = "ACTIVE, ACKNOWLEDGED"

typedef struct alarmTaskData{
    int* HVILState;         //0 = s1, 1 = s2, 2 = s3
    int* OvercurrentState;  //0 = s1, 1 = s2, 2 = s3
    int* HVOutOfRangeState; //0 = s1, 1 = s2, 2 = s3
    int* counter;   //counter starts = 1
    bool* alarmFlag; //true = run task false = skip task
    bool* forceAlarm;
}alarmData;

void alarmTask (void*);

#endif

#ifdef __cplusplus
}
#endif
