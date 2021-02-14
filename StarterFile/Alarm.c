#include <stdlib.h>
#include <stdbool.h>
#include "Alarm.h"

void updateHVILstate (int* HVILState, volatile bool* forceAlarm, bool* hvilReading){
    /****************
    * Function name: updateHVIL
    * Function inputs: a pointer to HVIL state
    * Function outputs: void
    * Function description: cycles through three different states for updateHVIL
                            cycling every 1 second
    * Author(s): Sam Quiring
    *****************/
    if(!*hvilReading && *HVILState == 0){
      *HVILState = 1;
      *forceAlarm = true;
    } else if(!*hvilReading){
      if(*forceAlarm){
        *HVILState = 1; 
      } else {
        *HVILState = 2;
      }
    }else
      *HVILState = 0;
}

void updateOvercurrentState (int* OvercurrentState, int* counter, volatile bool* forceAlarm, float* hvCurrent){
    /****************
    * Function name: update OvercurrentState
    * Function inputs: a pointer to Overcurrent state and a pointer to counter
    * Function outputs: void
    * Function description: cycles through three different states for Overcurrent
                            cycling every 2 seconds
    * Author(s): Sam Quiring
    *****************/

    if (*OvercurrentState == 0){
        if ((*hvCurrent < -5) || (*hvCurrent > 20)) {
           *OvercurrentState = 1;
           *forceAlarm = true;
        }
    } else if (*OvercurrentState == 1){
        if (*forceAlarm == false) {
            *OvercurrentState = 2;
        }
    }
    if ((*hvCurrent > 5) &&  (*hvCurrent < 20)) {
        *OvercurrentState = 0;
    }
}

void updateHVOutOfRange (int* HVOutOfRangeState, int* counter, volatile bool* forceAlarm, float* hvVoltage){
    /****************
    * Function name: updateHVOutOfRange
    * Function inputs: a pointer to HV out of range state and a ptr to counter
    * Function outputs: void
    * Function description: cycles through three different states for HVOutOfRange
                            cycling every 3 seconds
    * Author(s): Sam Quiring
    *****************/
    
    if(*HVOutOfRangeState == 1)
      *forceAlarm = true;

    if (*HVOutOfRangeState == 0){
        if ((*hvVoltage < 280) || (*hvVoltage > 405)) {
           *HVOutOfRangeState = 1;
           *forceAlarm = true;
        }
    } else if (*HVOutOfRangeState == 1){
        if (*forceAlarm == false) {
            *HVOutOfRangeState = 2;
        }
    }
    if ((*hvVoltage > 280) &&  (*hvVoltage < 405)) {
        *HVOutOfRangeState = 0;
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
    alarmData* data = (alarmData*) mData;
    if(*(data->alarmFlag)){
        updateHVILstate (data->HVILState,data->forceAlarm, data->hvilReading);
        updateOvercurrentState (data->OvercurrentState, data->counter, data->forceAlarm, data->hvCurrent);
        updateHVOutOfRange (data->HVOutOfRangeState, data->counter,data->forceAlarm, data->hvVoltage);
    }
    *(data->alarmFlag) = true;  //skips alarm for one clock cycle
}
