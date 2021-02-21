#include <stdlib.h>
#include <stdbool.h>
#include "contactor.h"
#include <Arduino.h>

void updateState(bool* contactorState, bool* batteryOn, bool* HVIL){
    /****************
    * Function name: updateState
    * Function inputs: A bool of the current contactor state and a bool
                        batteryOn
    * Function outputs: void
    * Function description: checks if the battery is on and updates
                            the contactor state accordingly
    * Author(s): Sam Quiring
    *****************/
    noInterrupts();
    if(*HVIL){
      if(*batteryOn){
          *contactorState = false;
      } else {
          *contactorState = true;
      }
    } else {
      *contactorState = true;
      *batteryOn = false;
    }
    interrupts();
}
void contactorTask(void* mData) {
    /****************
    * Function name: contactorTask
    * Function inputs: void* mData
    * Function outputs: void
    * Function description: calls all contactor functions
                            If contactorFlag is true runs all above, if false does not run any threads
    * Author(s): Sam Quiring
    *****************/
  	contactorData* data = (contactorData*) mData;
  	if(*(data->contactorFlag)){
        updateState(data->contactorState,data->batteryOn, data->HVILState);
  	}
   *(data->contactorFlag) = true;  //skips contactor for one clock cycle
}
