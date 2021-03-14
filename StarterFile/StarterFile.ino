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
#define BUFFER_SIZE 10 //the amount of denoicing we want to do
#define OffsetSample 20
#define gravity 175
//Task Control Blocks
TCB measurementTCB;         // Declare all TCBs
TCB touchScreenTCB;
TCB SOCTCB;
TCB contactorTCB;
TCB alarmTCB;
TCB remoteTerminalTCB;
TCB dataLoggingTCB;
TCB accelerometerTCB;



// all data
measurementData measure;    // Declare measurement data structure - defined in Measurement.h
touchScreenData touch;      // Declare touch screen data structure - defined in TouchScreen.h
SOCData SOC;                // Declare SOC data structure - defined in SOC.h
contactorData contactor;   // Declare contractor data structure - defined in contactor.h
alarmData alarm;            // Declare alarm data structure - defined in Alarm.h
remoteTerminalData remoteTerminal; //Declare remote terminal data structure - defined in RemoteTerminal.h
dataLoggingData dataLogging; //Declare data logging data structure - defined in DataLogging.h
accelerometerData accelerometer;

//measure global variables
//start with the number that was in their first state
float hvCurrent     = 0.0;
float hvVoltage     = 10.0;
float temperature   = 0.0;
bool HVIL           = false;
const byte hvilPin = 20; //the current state of pin 22. false = low true = high
const byte voltagePin = A10;  //the pins that read the inputs for our voltage, current and temperature
const byte currentPin = A12;
const byte tempPin = A14;

float currentMin = 0.0;
float currentMax = 0.0;
float voltageMin = -1;
float voltageMax = -1;
float temperatureMin = 0.0;
float temperatureMax = 0.0;

float chargeState = 0;
volatile bool measurementFlag = true;

//contactor global variables
bool contactorState = false; //true = open, false = closed
volatile bool contactorFlag = true;

//Alarm global variables
int HVILState = 0;
int OvercurrentState = 0;
int HVOutOfRangeState = 0;
volatile bool alarmFlag = true;
volatile bool alarmHVIL = false;
volatile bool alarmCurrent = false;
volatile bool alarmVoltage = false;

//SOC global variables
float stateOfCharge = 0.0;
volatile bool SOCFlag = true;

//touchscreen global variables
bool initialize = true; 
int touchState = 0; //0 = measurement, 1 = battery, 2 = Alarm | Battery screen is seperate
bool thingsChanged = true;
bool updateStates = true;
volatile bool touchScreenFlag = true;
volatile bool forceAlarm = false; //sets when the touchscreen is stuck in the alarmState
bool diffRate = false;

//remoteTerminal global variables
bool remoteTerminalFlag = true;
bool runStartFunc = true;
int incomingByte = -1; // the user inputted value

//dataLogging global variables
bool tempChangeMin = false;
bool tempChangeMax = false;
int tempAddressMin = 0;
int tempAddressMax = addressChange;
bool currentChangeMin = false;
bool currentChangeMax = false;
int currentAddressMin = addressChange*2;
int currentAddressMax = addressChange*3;
bool voltageChangeMin = false;
bool voltageChangeMax = false;
int voltageAddressMin = addressChange*4;
int voltageAddressMax = addressChange*5;
volatile bool dataLoggingFlag = true; //true = run task false = skip task

// Accelerometer global variables
volatile bool accelerometerFlag = true;
int xRawAcc = 0.0;
int yRawAcc = 0.0;
int zRawAcc = 0.0;
float xAcc = 0.0;
float yAcc = 0.0;
float zAcc = 0.0;
float xVel = 0.0;
float yVel = 0.0;
float zVel = 0.0;
float xDisplacement = 0.0;
float yDisplacement = 0.0;
float zDisplacement = 0.0;
float totalDistance = 0.0;
const byte xPin = A15;
const byte yPin = A14;
const byte zPin = A13;
float xAngle = 0.0;
float yAngle = 0.0;
float zAngle = 0.0;
float xAccBuff = 0.0;
float yAccBuff = 0.0;
float zAccBuff = 0.0;
float* xBuffer = (float*)calloc(BUFFER_SIZE,sizeof(float));
float* yBuffer = (float*)calloc(BUFFER_SIZE,sizeof(float));
float* zBuffer = (float*)calloc(BUFFER_SIZE,sizeof(float));
int xPtr = 0;
int yPtr = 0;
int zPtr = 0;
bool xBufferFull = false;
bool yBufferFull = false;
bool zBufferFull = false;
bool bigBufferFull = false;
int bufferSize = BUFFER_SIZE;
int xOffset = 0;
int yOffset = 0;
int zOffset = 0;
long timeTook = 10;
bool stationary = true;


//used to dynamically update the offsets if they are within a certain value settled
float* bigXBuffer = (float*)calloc(BUFFER_SIZE*20,sizeof(float));
float* bigYBuffer = (float*)calloc(BUFFER_SIZE*20,sizeof(float));
int bigPtrX = 0;
int bigPtrY = 0;
float bigX = 0;
float bigY = 0;
int bigbufferSize = 20*BUFFER_SIZE;



//multiple uses global variables
int counter = 1;
bool batteryOn = false; //true = ON false = OFF
const byte batteryPin = 40;
volatile bool timeBaseFlag = false;
volatile bool alarmReset = false; //this is the check to turn all alarm flags off 
volatile bool resetEEPROM = false;
float timeBase = 0.1;

long timer = 10;

int AddressToFloat(int address){
  /****************
    * Function name: AddressToFloat
    * Function inputs: int address
    * Function outputs: float
    * Function description: converts an address in EEPROM to a float
    * Author(s): Sam Quiring
    *****************/
    byte holder[4];
    byte tester;
    for(int i = 0; i < 4; i++){
      tester = EEPROM.read(address+i);
      holder[i] = tester;
    }
    float f;
    memcpy(&f, &holder, sizeof(f));
    return f;
    
}

void setup() { 
  /****************
    * Function name: setup
    * Function inputs: None
    * Function outputs: None
    * Function description: sets up the scheduler.
    * Author(s): Sam Quiring and Anders Hunt
    *****************/

  //memset(xBuffer,0,sizeof(xBuffer));  //sets the buffer to initially contain only zeros
  //memset(yBuffer,0,sizeof(yBuffer));
  //memset(zBuffer,0,sizeof(zBuffer));
  for(int i = 0; i < BUFFER_SIZE; i++){
    xBuffer[i] = 0;
    yBuffer[i] = 0;
    zBuffer[i] = 0;
  }

  for(int i = 0; i < BUFFER_SIZE*20; i++){
    bigXBuffer[i] = 0;
    bigYBuffer[i] = 0;
  }
  
  for(int i = 0; i < OffsetSample; i++){
      xOffset += analogRead(xPin);
      delay(100);
  }

  //initializes timerOne flag
  Timer1.initialize(10E+3);        //set the timer period to 100ms
  Timer1.attachInterrupt(timerISR); //Attach the interrupt service routine (ISR)
  Timer1.start();                   //Start the timer
     
  //initializes pin for HVIL so that it reads as an input
  pinMode(hvilPin,INPUT);
  //initializes pin for battery
  pinMode(batteryPin,OUTPUT);

  //pullup resistor and pin for HVIL voltage, current and temperature
  pinMode(voltagePin, INPUT_PULLUP);
  pinMode(currentPin, INPUT_PULLUP);
  pinMode(tempPin, INPUT_PULLUP);

  //sets all the values of min and max to the values in the EEPROM
  EEPROM.get(tempAddressMin,temperatureMin);
  EEPROM.get(tempAddressMax,temperatureMax);
  EEPROM.get(voltageAddressMin,voltageMin);
  EEPROM.get(voltageAddressMax,voltageMax);
  EEPROM.get(currentAddressMin,currentMin);
  EEPROM.get(currentAddressMax,currentMax);

  //temperatureMin = AddressToFloat(tempAddressMin);
  //temperatureMax = AddressToFloat(tempAddressMax);
  //voltageMin = AddressToFloat(voltageAddressMin);
  //voltageMax = AddressToFloat(voltageAddressMax);
 //currentMin = AddressToFloat(currentAddressMin);
  //currentMax = AddressToFloat(currentAddressMax);
  
  
  

  //attachInterrupt(digitalPinToInterrupt(hvilPin), alarmISR, RISING);  //creates an interrupt whenever hvilPin is high
  
  //setting all variables up so they are used in functions
  measure.hvilStatus = &HVIL;
  measure.hvilPin = &hvilPin;
  measure.temperature = &temperature;
  measure.hvCurrent = &hvCurrent;
  measure.hvVoltage = &hvVoltage;
  measure.counter = &counter;
  measure.measurementFlag = &measurementFlag;
  measure.tempPin = &tempPin;
  measure.currentPin = &currentPin;
  measure.voltagePin = &voltagePin;
  measure.tempChangeMin = &tempChangeMin;
  measure.tempChangeMax = &tempChangeMax;
  measure.currentChangeMin = &currentChangeMin;
  measure.currentChangeMax = &currentChangeMax;
  measure.voltageChangeMin = &voltageChangeMin;
  measure.voltageChangeMax = &voltageChangeMax;
  measure.currentMin = &currentMin;
  measure.currentMax = &currentMax;
  measure.voltageMin = &voltageMin;
  measure.voltageMax = &voltageMax;
  measure.temperatureMin = &temperatureMin;
  measure.temperatureMax = &temperatureMax;
  measure.resetEEPROM = &resetEEPROM;

  contactor.contactorState = &contactorState;
  contactor.batteryOn = &batteryOn;
  contactor.contactorFlag = &contactorFlag;
  contactor.hvilAlarm = &HVIL;
  contactor.overCurrentAlarm = &OvercurrentState;
  contactor.voltageAlarm = &HVOutOfRangeState;

  alarm.HVILState = &HVILState;
  alarm.OvercurrentState = &OvercurrentState;
  alarm.HVOutOfRangeState = &HVOutOfRangeState;
  alarm.counter = &counter;
  alarm.alarmFlag = &alarmFlag;
  alarm.forceAlarm = &forceAlarm;
  alarm.hvVoltage = &hvVoltage;
  alarm.hvCurrent = &hvCurrent;
  alarm.hvilReading = &HVIL;
  alarm.forceAlarmHVIL = &alarmHVIL;
  alarm.forceAlarmCurrent = &alarmCurrent;
  alarm.forceAlarmVoltage = &alarmVoltage;
  alarm.alarmReset = &alarmReset;

  SOC.stateOfCharge = &stateOfCharge;
  SOC.SOCFlag = &SOCFlag;
  SOC.hvCurrent = &hvCurrent;
  SOC.hvVoltage = &hvVoltage;
  SOC.temperature = &temperature;

  touch.touchState = &touchState;
  touch.HVILState = &HVILState;
  touch.OvercurrentState = &OvercurrentState;
  touch.HVOutOfRangeState = &HVOutOfRangeState;
  touch.batteryOn = &batteryOn;
  touch.temperature = &temperature;
  touch.hvCurrent = &hvCurrent;
  touch.hvVoltage = &hvVoltage;
  touch.SOCreading = &stateOfCharge;
  touch.nScreen = &updateStates;
  touch.initialize = &initialize;
  touch.HVIL = &HVIL;
  touch.csState = &contactorState;
  touch.touchScreenFlag = &touchScreenFlag;
  touch.forceAlarm = &forceAlarm;
  touch.alarmReset = &alarmReset;
  touch.diffRate = &diffRate;
  touch.counter = &counter;
  touch.xPosition = &xDisplacement;
  touch.yPosition = &yDisplacement;
  touch.zPosition = &zDisplacement;
  touch.totalDistance = &totalDistance;
  touch.xAngle = &xAngle;
  touch.yAngle = &yAngle;
  touch.zAngle = &zAngle;
  
  remoteTerminal.remoteTerminalFlag = &remoteTerminalFlag;
  remoteTerminal.runStartFunct = &runStartFunc;
  remoteTerminal.currentMin = &currentMin;
  remoteTerminal.currentMax = &currentMax;
  remoteTerminal.voltageMin = &voltageMin;
  remoteTerminal.voltageMax = &voltageMax;
  remoteTerminal.temperatureMin = &temperatureMin;
  remoteTerminal.temperatureMax = &temperatureMax;
  remoteTerminal.resetFlag = &resetEEPROM;
  remoteTerminal.incomingByte = &incomingByte;

  dataLogging.tempChangeMin = &tempChangeMin;
  dataLogging.tempChangeMax = &tempChangeMax;
  dataLogging.currentChangeMin = &currentChangeMin;
  dataLogging.currentChangeMax = &currentChangeMax;
  dataLogging.voltageChangeMin = &voltageChangeMin;
  dataLogging.voltageChangeMax = &voltageChangeMax;
  dataLogging.tempAddressMin = &tempAddressMin;
  dataLogging.tempAddressMax = &tempAddressMax;
  dataLogging.currentAddressMin = &currentAddressMin;
  dataLogging.currentAddressMax = &currentAddressMax;
  dataLogging.voltageAddressMin = &voltageAddressMin;
  dataLogging.voltageAddressMax = &voltageAddressMax;
  dataLogging.currentMin = &currentMin;
  dataLogging.currentMax = &currentMax;
  dataLogging.voltageMin = &voltageMin;
  dataLogging.voltageMax = &voltageMax;
  dataLogging.temperatureMin = &temperatureMin;
  dataLogging.temperatureMax = &temperatureMax;
  dataLogging.dataLoggingFlag = &dataLoggingFlag;
  dataLogging.resetEEPROM = &resetEEPROM;

  accelerometer.accelerometerFlag  = &accelerometerFlag;
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
  accelerometer.xAng = &xAngle;
  accelerometer.yAng = &yAngle;
  accelerometer.zAng = &zAngle;
  accelerometer.xPin = &xPin;
  accelerometer.yPin = &yPin;
  accelerometer.zPin = &zPin;
  accelerometer.xAccBuff = &xAccBuff;
  accelerometer.yAccBuff = &yAccBuff;
  accelerometer.zAccBuff = &zAccBuff;
  accelerometer.xBuffer = &xBuffer;
  accelerometer.yBuffer = &yBuffer;
  accelerometer.zBuffer = &zBuffer;
  accelerometer.xPtr = &xPtr;
  accelerometer.yPtr = &yPtr;
  accelerometer.zPtr = &zPtr;
  accelerometer.xBufferFull = &xBufferFull;
  accelerometer.yBufferFull = &yBufferFull;
  accelerometer.zBufferFull = &zBufferFull;
  accelerometer.bufferSize = &bufferSize;
  accelerometer.xOffset = &xOffset;
  accelerometer.yOffset = &yOffset;
  accelerometer.zOffset = &zOffset;
  accelerometer.bigXBuffer = &bigXBuffer;
  accelerometer.bigYBuffer = &bigYBuffer;
  accelerometer.bigPtrX = &bigPtrX;
  accelerometer.bigPtrY = &bigPtrY;
  accelerometer.bigX = &bigX;
  accelerometer.bigY = &bigY;
  accelerometer.bigBufferSize = &bigbufferSize;
  accelerometer.bigBufferFull = &bigBufferFull;
  accelerometer.stationary = &stationary;
  

  //setting TCB up so it is connected
  measurementTCB.taskDataPtr = &measure;
  measurementTCB.task = &measurementTask;
  measurementTCB.named = 7;

  contactorTCB.taskDataPtr = &contactor;
  contactorTCB.task = &contactorTask;
  contactorTCB.named = 2;

  alarmTCB.taskDataPtr = &alarm;
  alarmTCB.task = &alarmTask;
  alarmTCB.named = 3;

  SOCTCB.taskDataPtr = &SOC;
  SOCTCB.task = &SOCTask;
  SOCTCB.named = 1;

  touchScreenTCB.taskDataPtr = &touch;
  touchScreenTCB.task = &touchScreenTask;
  touchScreenTCB.named = 4;

  remoteTerminalTCB.taskDataPtr = &remoteTerminal;
  remoteTerminalTCB.task = &remoteTerminalTask;
  remoteTerminalTCB.named = 6;

  dataLoggingTCB.taskDataPtr = &dataLogging;
  dataLoggingTCB.task = &dataLoggingTask;
  dataLoggingTCB.named = 5;

  accelerometerTCB.taskDataPtr = &accelerometer;
  accelerometerTCB.task = &accelerometerTask;
  accelerometerTCB.named = 8;

  //creating the doubly linked list
  measurementTCB.prev = &accelerometerTCB;
  measurementTCB.next = &SOCTCB;
  SOCTCB.prev = &accelerometerTCB;
  SOCTCB.next = &contactorTCB;
  contactorTCB.prev = &SOCTCB;
  contactorTCB.next = &alarmTCB;
  alarmTCB.prev = &contactorTCB;
  alarmTCB.next = &touchScreenTCB;
  touchScreenTCB.prev = &alarmTCB;
  touchScreenTCB.next = &dataLoggingTCB;
  dataLoggingTCB.next = &remoteTerminalTCB;
  dataLoggingTCB.prev = &touchScreenTCB;
  remoteTerminalTCB.prev = &dataLoggingTCB;
  remoteTerminalTCB.next = NULL;
  
  accelerometerTCB.prev = NULL;
  accelerometerTCB.next = &measurementTCB;

  //Initialize serial communication
    Serial.begin(9600);
    Serial1.begin(9600);
    Serial1.setTimeout(1000);


}
unsigned long tester = 0;
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
    noInterrupts();
    delay(2000);
    for(int i = 0; i < OffsetSample; i++){
      zOffset += analogRead(zPin);
      yOffset += analogRead(yPin);
    delay(100);
    }
    xOffset = xOffset / OffsetSample;
    yOffset = yOffset / OffsetSample;
    zOffset = (zOffset / OffsetSample) - gravity;
    
    interrupts();
    while(1){
      if(timeBaseFlag){
           timer = millis(); //takes note of timing 
           timeBaseFlag = false;
           scheduler(&accelerometerTCB,taskArray, &counter);
           digitalWrite(batteryPin,!contactorState);
           counter++;
           timeTook = millis() - timer;
           if(timeTook < 10)
            timeTook = 10;
           //Serial.println(timeTook);
           
           
          // Serial.print("Y buffered big: ");
          // Serial.print(bigY);
           Serial.print(" Y unbuffered: ");
           Serial.print(yAcc);
           Serial.print(" Y buffered: ");
           Serial.println(yAccBuff);
           Serial.print(" X unbuffered: ");
           Serial.print(xAcc);
           Serial.print(" X buffered: ");
           Serial.println(xAccBuff);
           Serial.print(" total distance: ");
           Serial.println(totalDistance);
           //Serial.print("X buffered big: ");
           //Serial.println(bigX);
               
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
  accelerometerFlag = false;
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
