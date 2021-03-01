#include <Arduino.h>
#include <EEPROM.h>
#include "DataLogging.h"

void updateEEPROM(float* measurement, int* address){
    /****************
    * Function name: updateEEPROM
    * Function inputs: float* measurement
    * Function outputs: void
    * Function description: updates the current value at the current address
    * Author(s): Sam Quiring
    *****************/
    EEPROM.put(*address,*measurement);
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
       if(*(data->tempChange)){
        updateEEPROM(data->temperature,data->tempAdress);
        *(data->tempChange) = false;
       }
       if(*(data->currentChange)){
        updateEEPROM(data->hvCurrent,data->currentAddress);
        *(data->currentChange) = false;
       }
       if(*(data->voltageChange)){
        updateEEPROM(data->hvVoltage,data->voltageAddress);
        *(data->voltageChange) = false;
       }
    }
    *(data->dataLoggingFlag) = true;  //skips measurement for one clock cycle
}
