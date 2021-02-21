#include <stdlib.h>
#include <stdbool.h>
#include "Alarm.h"
#include <Arduino.h>

void updateHVILstate (int* HVILState, volatile bool* forceAlarm, bool* hvilReading){
    /****************
    * Function name: updateHVIL
    * Function inputs: a pointer to HVIL state, a pointer to an alarm for HVIL, and a pointer to
    *                 our current HVIL reading
    * Function outputs: void
    * Function description: Updates the state of the HVIL depending on if our HVIL reading is
    *                       false and if we have acknowledged the alarm 
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
    }else {
      *HVILState = 0;
      *forceAlarm = false;
    }
}

void updateOvercurrentState (int* OvercurrentState, volatile bool* forceAlarm, float* hvCurrent){
    /****************
    * Function name: update OvercurrentState
    * Function inputs: a pointer to Overcurrent state, a pointer to an alarm for overcurrent, and a pointer to
    *                   our current reading
    * Function outputs: void
    * Function description: Updates the state of the current alarm depending on if our current is within our 
    *                       specified range and in the event it is outside the range updates states depending on if
    *                       we have acknowledged the alarm 
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
        *forceAlarm = false;
    }
}

void updateHVOutOfRange (int* HVOutOfRangeState, volatile bool* forceAlarm, float* hvVoltage){
    /****************
    * Function name: updateHVOutOfRange
    * Function inputs: a pointer to voltage state, a pointer to an alarm for voltage, and a pointer to
    *                   our voltage reading
    * Function outputs: void
    * Function description: Updates the state of the voltage alarm depending on if our voltage is within our 
    *                       specified range and in the event it is outside the range updates states depending on if
    *                       we have acknowledged the alarm 
    * Author(s): Sam Quiring
    *****************/

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
        *forceAlarm = false;
    }
}

void updateAlarms(volatile bool* voltage, volatile bool* current, volatile bool* HVIL, volatile bool* forceAlarm, volatile bool* alarmReset){
  /****************
    * Function name: updateAlarms
    * Function inputs: a pointer to the voltageAlarm, currentAlarm, HVILAlarm, forceAlarm, and AlarmReset
    * Function outputs: void
    * Function description: sets the value of our forceAlarm depending on if any of the other alarms are turned on
    *                       it also will turn off all alarms if alarm reset is called/true
    * Author(s): Sam Quiring
    *****************/
  if(*alarmReset){
    *voltage = false;
    *current = false;
    *HVIL = false;
    *alarmReset = false;
  }
  *forceAlarm = *voltage || *current || *HVIL;
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
        updateHVILstate (data->HVILState,data->forceAlarmHVIL, data->hvilReading);
        updateOvercurrentState (data->OvercurrentState, data->forceAlarmCurrent, data->hvCurrent);
        updateHVOutOfRange (data->HVOutOfRangeState, data->forceAlarmVoltage, data->hvVoltage);
        updateAlarms(data->forceAlarmVoltage,data->forceAlarmCurrent,data->forceAlarmHVIL,data->forceAlarm,data->alarmReset);
    }
    *(data->alarmFlag) = true;  //skips alarm for one clock cycle
}
