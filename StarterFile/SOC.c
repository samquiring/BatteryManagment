#include <stdlib.h>
#include <stdbool.h>
#include "SOC.h"
// #include <Arduino.h>

#define COLUMNS_NUMBER 6
#define ROWS_NUMBER 5

// VO= hvVOltage + 0.5 * hvCurrent

void updateSOC(float* SOCreading, float* hvVoltage, float* hvCurrent, float* temperature) {
  /****************
    * Function name: updateSOC
    * Function inputs: int Soc reading
    * Function outputs: void
    * Function description: soc is always set to 0
    * Author(s): Anders Hunt
    *****************/

  
    


  float socArr[ROWS_NUMBER][COLUMNS_NUMBER] = {
    {-10, 200, 250, 300, 350, 400},
    {-10, 0, 10, 35, 100, 100},
    {0, 0, 0, 20, 80, 100},
    {25, 0, 0, 10, 60, 100},
    {45, 0, 0, 0, 50, 100}
  };

  float VOC = (float) *hvVoltage + 0.5 * *hvCurrent;
  float* soc = &socArr[0][0];
  int xRangeHigh;
  int xRangeLow;
  int yRangeHigh;
  int yRangeLow;
  int voltageLow;
  int voltageHigh;
  int tempLow;
  int tempHigh;
  bool passed = false;
  
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
    if ((VOC < *(soc + COLUMNS_NUMBER * j)) && (!(passed))){
        yRangeHigh = j;
        yRangeLow = j - 1;
        tempLow = *(soc + COLUMNS_NUMBER * yRangeLow);
        tempHigh = *(soc + COLUMNS_NUMBER * yRangeHigh);
        passed = true;
    }
  }


    float SOCp1 = *(soc + (xRangeLow * COLUMNS_NUMBER + yRangeLow));
    float SOCp2 = *(soc + (xRangeHigh * COLUMNS_NUMBER + yRangeLow));
    float SOCp3 = *(soc + (xRangeLow * COLUMNS_NUMBER + yRangeHigh));
    float SOCp4 = *(soc + (xRangeHigh * COLUMNS_NUMBER + yRangeHigh));

    float y_offset = (*temperature - tempLow) / (tempHigh - tempLow);
    float x_offset = (VOC - voltageLow) / (voltageHigh - voltageLow);

    float socMidP13 = SOCp1 + y_offset * (SOCp3 - SOCp1);
    float socMidP24 = SOCp2 + y_offset * (SOCp4 - SOCp2);


    *SOCreading = socMidP13 + x_offset * (socMidP24 - socMidP13);

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
