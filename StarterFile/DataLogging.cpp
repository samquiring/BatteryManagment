#include <Arduino.h>
#include <EEPROM.h>
#include "DataLogging.h"

byte* floatToByte(float fl){
   /****************
    * Function name: floatToByte
    * Function inputs: float fl
    * Function outputs: byte*
    * Function description: converts a float to a byte array
    * Author(s): Sam Quiring
    *****************/
    unsigned int changed = (*(int*)&fl); //converts bit for bit the float to an int
    byte list[4];
    for(int i = 0; i < 4; i++){
      list[i] = (changed >> 8 * i) & 0xFF; //bitmask to take a single byte of data
    }
    return list;
}

void updateMeasurement(float* Min, int* MinAddress, bool* MinChanged, float* Max, int* MaxAddress, bool* MaxChanged, volatile bool* resetEEPROM){
    /****************
    * Function name: updateMeasurement
    * Function inputs: float* Min, int* MinAddress, bool* MinChanged, float* Max, int* MaxAddress, bool* MaxChanged
    * Function outputs: void
    * Function description: checks if measurement max or min has changed and updates it if it has
    * Author(s): Sam Quiring
    *****************/
    if(*MinChanged || *resetEEPROM){
      EEPROM.put(*MinAddress,*Min);
       *MinChanged = false;
    }
    if(*MaxChanged || *resetEEPROM){
      EEPROM.put(*MaxAddress,*Max);
      *MaxChanged = false;
    }
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
