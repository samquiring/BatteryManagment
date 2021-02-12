#include <stdlib.h>
#include <stdbool.h>
#include "Measurement.h"
#include <Arduino.h>

void updateHVIL(bool* hvilReading, const byte* pin) {
     /****************
    * Function name: updateHVIL1
    * Function inputs: a bool hvilReading and a bool pin
    * Function outputs: void
    * Function description: updates hvilReading to false if
                            pin is true and true if pin is false
    * Author(s): Anders Hunt
    *****************/

    if (digitalRead(*pin) == HIGH) {
      *hvilReading = false;
    } else {
      *hvilReading = true;
    } 
    return;
}

void updateTemperature(float* temperatureReading, int* aPin1) {
    /****************
    * Function name: updateTemperature
    * Function inputs: a float of the temperature
    * Function outputs: void
    * Function description: cycles through three values of temperature round robin style
    * Author(s): Anders Hunt
    *****************/

  temperatureReading = ((analogRead(*aPin1) / 1023.0) * (11.0)) - 10.0;
    
  	return;
}

void updateHvCurrent(float* currentReading, int* counter, int* aPin2) {
    /****************
    * Function name: updateHvCurrent
    * Function inputs: float of the currentReading and an int of counter
    * Function outputs: void
    * Function description: updates currentReading to 1 of three values every two cycles
                            round robin style
    * Author(s): Anders Hunt
    *****************/

    *currentReading = (analogRead(*aPin2) / 1023.0) * 10.0 - 25.0;
    
  	return;
}

void updateHvVoltage(float* voltageReading, int* counter, int* aPin3) {
    /****************
    * Function name: updateHvVoltage
    * Function inputs: float of the voltage reading and int counter
    * Function outputs: void
    * Function description: updates the voltage to 1 of three values every three cycles
                            round robin style
    * Author(s): Anders Hunt
    *****************/


    *voltageReading = ((analogRead(*aPin3) / 1023.0) * 90.0);
    
  	return;
}

void measurementTask(void* mData) {
    /****************
    * Function name: measurementTask
    * Function inputs: void* mData
    * Function outputs: void
    * Function description: calls all functions within the measurement file
    * Author(s): Anders Hunt
    *****************/
  	measurementData* data = (measurementData*) mData;

  	// Update all sensors
  	updateHVIL(data->hvilStatus, data->hvilPin);
  	updateTemperature(data->temperature, data->tempPin);
  	updateHvCurrent(data->hvCurrent, data->counter, data->currentPin);
  	updateHvVoltage(data->hvVoltage, data->counter, data->voltagePin);
    *(data->finishedFlag) = true;
}
