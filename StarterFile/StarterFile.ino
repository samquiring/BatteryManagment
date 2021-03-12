#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Measurement.h"
#include "TaskControlBlock.h"
#include "contactor.h"
#include "Alarm.h"
#include "SOC.h"
#include "TouchScreen.h"
#include "scheduler.c"
#include "RemoteTerminal.h"
#include "DataLogging.h"
#include <TimerOne.h>
#include <EEPROM.h>
#include "Accelerometer.h"

#define CHARSIZE 60 //size of the lines below
#define addressChange 8 //the distance between addresses for our eeprom
@@ -23,6 +24,7 @@ TCB contactorTCB;
TCB alarmTCB;
TCB remoteTerminalTCB;
TCB dataLoggingTCB;
TCB AccelerometerTCB;



@@ -34,6 +36,7 @@ contactorData contactor;   // Declare contractor data structure - defined in con
alarmData alarm;            // Declare alarm data structure - defined in Alarm.h
remoteTerminalData remoteTerminal; //Declare remote terminal data structure - defined in RemoteTerminal.h
dataLoggingData dataLogging; //Declare data logging data structure - defined in DataLogging.h
accelerometerData accelerometer;

//measure global variables
//start with the number that was in their first state
@@ -102,13 +105,32 @@ int voltageAddressMin = addressChange*4;
int voltageAddressMax = addressChange*5;
volatile bool dataLoggingFlag = true; //true = run task false = skip task

// Accelerometer global variables
float xRawAcc;
float yRawAcc;
float zRawAcc;
float xAcc;
float yAcc;
float zAcc;
float xVel;
float yVel;
float zVel;
float xDisplacement;
float yDisplacement;
float zDisplacement;
float totalDistance;
const byte xPin = A15;
const byte yPin = A14;
const byte zPin = A13;

//multiple uses global variables
int counter = 1;
bool batteryOn = false; //true = ON false = OFF
const byte batteryPin = 40;
volatile bool timeBaseFlag = false;
volatile bool alarmReset = false; //this is the check to turn all alarm flags off 
volatile bool resetEEPROM = false; 
volatile bool resetEEPROM = false;
float timeBase = (100E+3);

int AddressToFloat(int address){
  /****************
@@ -277,6 +299,21 @@ void setup() {
  dataLogging.dataLoggingFlag = &dataLoggingFlag;
  dataLogging.resetEEPROM = &resetEEPROM;

  accelerometer.xRawAcc = &xRawAcc;
  accelerometer.yRawAcc = &yRawAcc;
  accelerometer.zRawAcc = &zRawAcc;
  accelerometer.xAcc = &xAcc;
  accelerometer.yAcc = &yAcc;
  accelerometer.zAcc = &zAcc;
  accelerometer.xVel = &xVel;
  accelerometer.yVel = &yVel;
  accelerometer.zVel = &zVel;
  accelerometer.xDisplacement = &xDisplacement;
  accelerometer.yDisplacement = &yDisplacement;
  accelerometer.zDisplacement = &zDisplacement;
  accelerometer.totalDistance = &totalDistance;
  accelerometer.timeBase = &timeBase;


  //setting TCB up so it is connected
  measurementTCB.taskDataPtr = &measure;
@@ -307,6 +344,10 @@ void setup() {
  dataLoggingTCB.task = &dataLoggingTask;
  dataLoggingTCB.named = 5;

  accelerometerTCB.taskDataPtr = &accelerometer;
  accelerometerTCB.task = &accelerometerTask;
  accelerometerTCB.named = ;

  //creating the doubly linked list
  measurementTCB.prev = NULL;
  measurementTCB.next = &SOCTCB;
@@ -322,6 +363,8 @@ void setup() {
  dataLoggingTCB.prev = &touchScreenTCB;
  remoteTerminalTCB.prev = &dataLoggingTCB;
  remoteTerminalTCB.next = NULL;
  accelerometerTCB.prev = ;
  accelerometerTCB.next = ;

  //Initialize serial communication
    Serial.begin(9600);
    Serial1.begin(9600);
    Serial1.setTimeout(1000);
}
TCB* taskArray[] = {&touchScreenTCB,&remoteTerminalTCB,&dataLoggingTCB,&measurementTCB,&SOCTCB,&contactorTCB,&alarmTCB};
void loop() {
  /****************
    * Function name:    loop
    * Function inputs:  Sensor data, touch input
    * Function outputs: Display data and lights indicating alarm status, contactor status, sensor data, & state of charge
    * Function description: This is a doubly linked list scheduler to run a series of tasks
    *                       that create a user interface to a battery management system
    * Author(s): Sam Quiring
    *****************/
    while(1){
      if(timeBaseFlag){
           timeBaseFlag = false;
           scheduler(&measurementTCB,taskArray, &counter);
           digitalWrite(batteryPin,!contactorState);
           counter++;
      }
    } 
}
void timerISR(){          //interrupts service routine
   /****************
    * Function name:    timerISR
    * Function inputs:  none
    * Function outputs: void
    * Function description: sets the timeBaseFlag to true when executed
    * Author(s): Sam Quiring
    *****************/
  noInterrupts();
  timeBaseFlag = true;   //set timerISR flag
  interrupts();
}
void alarmISR(){
  /****************
    * Function name:    AlarmISR
    * Function inputs:  none
    * Function outputs: void
    * Function description: when triggered sets all flags accept alarm to false so that we immediately enter the alarm state
    * Author(s): Sam Quiring
    *****************/
  noInterrupts();
  measurementFlag = false;
  SOCFlag = false;
  contactorFlag = false;
  touchScreenFlag = false;
  remoteTerminalFlag = false;
  dataLoggingFlag = false;
  forceAlarm = true;
  Serial.println("Alarm Flag Raised");
  interrupts();
}
