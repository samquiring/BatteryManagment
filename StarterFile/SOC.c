#include <stdlib.h>
#include <stdbool.h>
#include "SOC.h"

void updateSOC(int* SOCreading) {
  /****************
    * Function name: updateSOC
    * Function inputs: int Soc reading
    * Function outputs: void
    * Function description: soc is always set to 0
    * Author(s): Anders Hunt
    *****************/
  *SOCreading = 0;
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
        updateSOC(data->stateOfCharge);
    }
}
