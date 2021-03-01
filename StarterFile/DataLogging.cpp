#include <Arduino.h>
#include <EEPROM.h>
#include "DataLogging.h"

void updateMeasurement(float* Min, int* MinAddress, bool* MinChanged, float* Max, int* MaxAddress, bool* MaxChanged, volatile bool* resetEEPROM){
    /****************
    * Function name: updateMeasurement
    * Function inputs: float* Min, int* MinAddress, bool* MinChanged, float* Max, int* MaxAddress, bool* MaxChanged
    * Function outputs: void
    * Function description: checks if measurement max or min has changed and updates it if it has
    * Author(s): Sam Quiring
    *****************/
    if(*MinChanged){
       EEPROM.put(*MinAddress,*Min);
       *MinChanged = false;
    }
    if(*MaxChanged){
      EEPROM.put(*MaxAddress,*Max);
      *MaxChanged = false;
    }

    //min and max should be set to the reset values because measurement will be before this
    if(*resetEEPROM){
      EEPROM.put(*MinAddress,*Min);
      EEPROM.put(*MaxAddress,*Max);
    }
}

void checkUpdates(){
  
}

void dataLoggingTask(void* mData) {
    /****************
    * Function name: dataLoggingTask
    * Function inputs: void* mData
    * Function outputs: void
    * Function description: calls all functions within the dataLogging file that need to be updated
                            If dataLoggingFlag is true runs all above, if false does not run any threads
    * Author(s): Sam Quiring
    *****************/
  	dataLoggingData* data = (dataLoggingData*) mData;
    if(*(data->dataLoggingFlag)){
      updateMeasurement(data->temperatureMin,data->tempAddressMin,data->tempChangeMin,data->temperatureMax,data->tempAddressMax,data->tempChangeMax, data->resetEEPROM);
      updateMeasurement(data->currentMin,data->currentAddressMin,data->currentChangeMin,data->currentMax,data->currentAddressMax,data->currentChangeMax, data->resetEEPROM);
      updateMeasurement(data->voltageMin,data->voltageAddressMin,data->voltageChangeMin,data->voltageMax,data->voltageAddressMax,data->voltageChangeMax, data->resetEEPROM);
      *(data->resetEEPROM) = false;
      
    }
    *(data->dataLoggingFlag) = true;  //skips measurement for one clock cycle
}
