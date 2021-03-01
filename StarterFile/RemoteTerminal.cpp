#include <Arduino.h>
#include "RemoteTerminal.h"

void printOptions(){
    /****************
    * Function name: printOptions
    * Function inputs: void
    * Function outputs: void
    * Function description: prints out the terminal options on startup or everytime after a user has selected a value
    * Author(s): Sam Quiring
    *****************/
    Serial.println("[1] Reset EEPROM");
    Serial.println("[2] HV Current Range [Hi, Lo]");
    Serial.println("[3] HV Voltage Range [Hi, Lo]");
    Serial.println("[4] Temperature Range [Hi, Lo]");
    Serial.println();
    Serial.print("Enter your menu choice [1-4]: ");
}


void remoteTerminalTask(void* mData) {
    /****************
    * Function name: remoteTerminalTask
    * Function inputs: void* mData
    * Function outputs: void
    * Function description: calls all functions needed within the remote terminal file
                            If remoteTerminalFlag is true runs all above, if false does not run any threads
    * Author(s): Sam Quiring
    *****************/
  	remoteTerminalData* data = (remoteTerminalData*) mData;
    if(*(data->remoteTerminalFlag)){
        printOptions();
    }
    *(data->remoteTerminalFlag) = true;  //skips measurement for one clock cycle
}
