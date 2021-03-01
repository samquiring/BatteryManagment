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

void userInput(int* incomingByte){
  *incomingByte = Serial.read();
}

void resetEEPROM(volatile bool* resetFlag){
  Serial.println("resetting all stored high and low values in EEPROM");
  *resetFlag = true;
}

void printRange(float* minVal, float* maxVal, String type, bool* runStartFunc){
  Serial.println();
  Serial.print("Your max recorded ");
  Serial.print(type);
  Serial.print(" is : ");
  Serial.println(*maxVal);
  Serial.print("Your min recorded ");
  Serial.print(type);
  Serial.print(" is : ");
  Serial.println(*minVal);
  Serial.println();
  *runStartFunc = true;
  
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
      if(*(data->runStartFunct)){
        printOptions();
        *(data->runStartFunct) = false;
      }
      userInput(data->incomingByte);
      //all of the possible user inputs
      if(*data->incomingByte == 49){
        resetEEPROM(data->resetFlag);
      } else if(*data->incomingByte == 50){
        printRange(data->currentMin,data->currentMax,"current",data->runStartFunct);
      } else if(*data->incomingByte == 51){
        printRange(data->voltageMin,data->voltageMax,"voltage",data->runStartFunct);
      } else if(*data->incomingByte == 52) {
        printRange(data->temperatureMin,data->temperatureMax,"temperature",data->runStartFunct);
      }
    }
    *(data->remoteTerminalFlag) = true;  //skips measurement for one clock cycle
}
