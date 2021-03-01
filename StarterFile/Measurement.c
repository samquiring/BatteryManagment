#include <stdlib.h>
#include <stdbool.h>
#include "Measurement.h"
#include <Arduino.h>
#define pinOffset 13.0 //This is the min value of the analog pin because resistance can't be 0hm on the potentionmeter
#define pinMax 970.0   //This is the max value of the analog pin because resistance can't be infinity on the potentionmeter
#define VOLTAGEMAX 450.0 //the max value of our voltage
#define currentOffset -25.0 //the amount we need to offset our current output by
#define currentSize 50.0 //-25 to 25 spans 50
#define tempOffset -10.0 //the amount we need to offset our temperature output by
#define tempSize 55.0 //-10 to 45 spans 55

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
}

void updateTemperature(float* temperatureReading, const byte* aPin1) {
    /****************
    * Function name: updateTemperature
    * Function inputs: a float of the temperature, and 
    *                  a pointer to the temperature reading pin
    * Function outputs: void
    * Function description: updates the value of the temperature output
    *                       in terms of degrees from the voltage input values from the given pin
    * Author(s): Anders Hunt
    *****************/
  *temperatureReading = (float)((((analogRead(*aPin1)-pinOffset) / pinMax) * tempSize) + tempOffset);
}

void updateHvCurrent(float* currentReading, const byte* aPin2) {
    /****************
    * Function name: updateHvCurrent
    * Function inputs: float of the currentReading and
    *                  a pointer to the current reading pin
    * Function outputs: void
    * Function description: updates the value of the current output
    *                       in terms of Amps from the voltage input values from the given pin
    * Author(s): Anders Hunt
    *****************/
    *currentReading = ((analogRead(*aPin2)-pinOffset) / pinMax) * currentSize + currentOffset;
}

void updateHvVoltage(float* voltageReading, const byte* aPin3) {
    /****************
    * Function name: updateHvVoltage
    * Function inputs: float of the voltageReading and
    *                  a pointer to the voltage reading pin
    * Function outputs: void
    * Function description: updates the value of the voltage output
    *                       in terms of Volts from the voltage input values from the given pin
    * Author(s): Anders Hunt
    *****************/
    *voltageReading = ((analogRead(*aPin3)-pinOffset) / pinMax) * VOLTAGEMAX;
}

void checkExtremes(float* reading, float* readingMax, float* readingMin, bool* newMax, bool* newMin){
 /****************
    * Function name: checkExtremes
    * Function inputs: float* reading, float* readingMax, float* readingMin, bool* newMax, bool* newMin
    * Function outputs: void
    * Function description: checks the current reading to see if it is a new min or max and updates to that min
    *                       or max if true
    * Author(s): Sam Quiring
    *****************/
    
    if(*reading < *readingMin){
      *readingMin = *reading;
      *newMin = true;
    } else if(*reading > *readingMax){
      *readingMax = *reading;
      *newMax = true;
    }
  
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
      updateTemperature(data->temperature, data->tempPin);
      updateHvCurrent(data->hvCurrent, data->currentPin);
      updateHvVoltage(data->hvVoltage, data->voltagePin);
      checkExtremes(data->temperature, data->temperatureMax, data->temperatureMin, data->tempChangeMax, data->tempChangeMin);
      checkExtremes(data->hvVoltage, data->voltageMax, data->voltageMin, data->voltageChangeMax, data->voltageChangeMin);
      checkExtremes(data->hvCurrent, data->currentMax, data->currentMin, data->currentChangeMax, data->currentChangeMin);
    }
    *(data->measurementFlag) = true;  //skips measurement for one clock cycle
}
