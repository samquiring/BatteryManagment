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

void updateTemperature(float* temperatureReading) {
    /****************
    * Function name: updateTemperature
    * Function inputs: a float of the temperature
    * Function outputs: void
    * Function description: cycles through three values of temperature round robin style
    * Author(s): Anders Hunt
    *****************/
  if (*temperatureReading == -10) {
    *temperatureReading = 5;
  } else if (*temperatureReading == 5) {
    *temperatureReading = 25;
  } else {
    *temperatureReading = -10;
  }

  	return;
}

void updateHvCurrent(float* currentReading, int* counter) {
    /****************
    * Function name: updateHvCurrent
    * Function inputs: float of the currentReading and an int of counter
    * Function outputs: void
    * Function description: updates currentReading to 1 of three values every two cycles
                            round robin style
    * Author(s): Anders Hunt
    *****************/

    if ((*counter % 2 == 0) && (*currentReading == -20)) {
      *currentReading = 0;
    } else if ((*counter % 2 == 0) && (*currentReading == 0)) {
      *currentReading = 20;
    } else if ((*counter % 2 == 0) && (*currentReading == 20)) {
      *currentReading = -20;
    }

  	return;
}

void updateHvVoltage(float* voltageReading, int* counter) {
    /****************
    * Function name: updateHvVoltage
    * Function inputs: float of the voltage reading and int counter
    * Function outputs: void
    * Function description: updates the voltage to 1 of three values every three cycles
                            round robin style
    * Author(s): Anders Hunt
    *****************/
    if ((*counter % 3 == 0) && (*voltageReading == 10)) {
      *voltageReading = 150;
    } else if ((*counter % 3 == 0) && (*voltageReading == 150)) {
      *voltageReading = 450;
    } else if ((*counter % 3 == 0) && (*voltageReading == 450)) {
      *voltageReading = 10;
    }
  	return;
}

void measurementTask(void* mData) {
    /****************
    * Function name: measurementTask
    * Function inputs: void* mData
    * Function outputs: void
    * Function description: calls all functions within the measurement file
                            If measurementFlag is true runs all above, if false does not run any threads
    * Author(s): Anders Hunt
    *****************/
  	measurementData* data = (measurementData*) mData;
    if(*(data->measurementFlag)){
        // Update all sensors
        updateHVIL(data->hvilStatus, data->hvilPin);
        updateTemperature(data->temperature);
        updateHvCurrent(data->hvCurrent, data->counter);
        updateHvVoltage(data->hvVoltage, data->counter);
    }
}
