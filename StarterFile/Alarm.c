#include <stdlib.h>
#include <stdbool.h>
#include "Alarm.h"

void updateHVILstate (int* HVILState){
    /****************
    * Function name: updateHVIL
    * Function inputs: a pointer to HVIL state
    * Function outputs: void
    * Function description: cycles through three different states for updateHVIL
                            cycling every 1 second
    * Author(s): Sam Quiring
    *****************/
    if(*HVILState < 2){
        *HVILState += 1;
    } else {
        *HVILState = 0;
    }
}

void updateOvercurrentState (int* OvercurrentState, int* counter){
    /****************
    * Function name: update OvercurrentState
    * Function inputs: a pointer to Overcurrent state and a pointer to counter
    * Function outputs: void
    * Function description: cycles through three different states for Overcurrent
                            cycling every 2 seconds
    * Author(s): Sam Quiring
    *****************/
    if(*counter%2 == 0){
        if(*OvercurrentState < 2){
            *OvercurrentState += 1;
        } else {
            *OvercurrentState = 0;
        }
    }
}

void updateHVOutOfRange (int* HVOutOfRangeState, int* counter){
    /****************
    * Function name: updateHVOutOfRange
    * Function inputs: a pointer to HV out of range state and a ptr to counter
    * Function outputs: void
    * Function description: cycles through three different states for HVOutOfRange
                            cycling every 3 seconds
    * Author(s): Sam Quiring
    *****************/
    if(*counter%3 == 0){
        if(*HVOutOfRangeState < 2){
            *HVOutOfRangeState += 1;
        } else {
            *HVOutOfRangeState = 0;
        }
    }
}

void alarmTask(void* mData){
    /****************
    * Function name: alarmTask
    * Function inputs: void* mData
    * Function outputs: void
    * Function description: calls all functions within class to update all variables
                            If AlarmFlag is true runs all above, if false does not run any threads
    * Author(s): Sam Quiring
    *****************/
    //runs if our alarmFlag is up
    if(*alarmFlag){
        alarmData* data = (alarmData*) mData;
        updateHVILstate (data->HVILState);
        updateOvercurrentState (data->OvercurrentState, data->counter);
        updateHVOutOfRange (data->HVOutOfRangeState, data->counter);
    }
}
