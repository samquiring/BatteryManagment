#include <stdlib.h>
#include <stdbool.h>
#include "SOC.h"
#include <Arduino.h>

#define COLUMNS_NUMBER 6
#define ROWS_NUMBER 5



void updateSOC(float* SOCreading, float* hvVoltage, float* hvCurrent, float* temperature) {
  /****************
    * Function name: updateSOC
    * Function inputs: int Soc reading
    * Function outputs: void
    * Function description: soc is always set to 0
    * Author(s): Anders Hunt
    *****************/

  
    


  float socArr[ROWS_NUMBER][COLUMNS_NUMBER] = {
    {-10.0, 200.0, 250.0, 300.0, 350.0, 400.0},
    {-10.0, 0.0, 10.0, 35.0, 100.0, 100.0},
    {0.0, 0.0, 0.0, 20.0, 80.0, 100.0},
    {25.0, 0.0, 0.0, 10.0, 60.0, 100.0},
    {45.0, 0.0, 0.0, 0.0, 50.0, 100.0}
  };

  float VOC = (float) *hvVoltage + 0.5 * *hvCurrent;
  float* soc = &socArr[0][0];
  int xRangeHigh;
  int xRangeLow;
  int yRangeHigh;
  int yRangeLow;
  float voltageLow;
  float voltageHigh;
  float tempLow;
  float tempHigh;
  bool passed = false;
 // *SOCreading = VOC;

  if (VOC > 400.0) {
    *SOCreading = 100.0;
  } else if (VOC < 200.0) {
    *SOCreading = 0.0;
  } else {
  

    for (int i = 0; i < COLUMNS_NUMBER; i++) {
      if ((VOC < *(soc + i)) && (!(passed))){
        xRangeHigh = i;
        xRangeLow = i - 1;
        voltageLow = *(soc + xRangeLow);
        voltageHigh = *(soc + xRangeHigh);
        passed = true;
      }
    }
    
    passed = false;

    for (int j = 0; j < ROWS_NUMBER; j++) {
      if ((*temperature < *(soc + COLUMNS_NUMBER * j)) && (!(passed))){
        yRangeHigh = j;
        yRangeLow = j - 1;
        tempLow = *(soc + COLUMNS_NUMBER * yRangeLow);
        tempHigh = *(soc + COLUMNS_NUMBER * yRangeHigh);
        passed = true;     
       }
    }
    passed = false;
    
    float SOCp1 = *(soc + (yRangeLow * COLUMNS_NUMBER + xRangeLow));
    float SOCp2 = *(soc + (yRangeHigh * COLUMNS_NUMBER + xRangeLow));
    float SOCp3 = *(soc + (yRangeLow * COLUMNS_NUMBER + xRangeHigh));
    float SOCp4 = *(soc + (yRangeHigh * COLUMNS_NUMBER + xRangeHigh));

    float yOffset = ((*temperature - tempLow) / (tempHigh - tempLow));
    float xOffset = ((VOC - voltageLow) / (voltageHigh - voltageLow));

    float socMidP13 = SOCp1 + xOffset * (SOCp3 - SOCp1);
    float socMidP24 = SOCp2 + xOffset * (SOCp4 - SOCp2);


    *SOCreading = socMidP13 + yOffset * (socMidP24 - socMidP13);
  }
}

void SOCTask(void* sData) {
    /****************
    * Function name: SOCTask
    * Function inputs: void* sData
    * Function outputs: void
    * Function description: calls all functions within SOC
                            If SOCFlag is true runs all above, if false does not run any threads
    * Author(s): Anders Hunt
    *****************/
    SOCData* data = (SOCData*) sData;
    if(*(data->SOCFlag)){
        updateSOC(data->stateOfCharge, data->hvVoltage, data->hvCurrent, data->temperature);
    }
    *(data->SOCFlag) = true;  //skips SOC for one clock cycle
}
