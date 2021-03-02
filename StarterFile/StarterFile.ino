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

#define CHARSIZE 60 //size of the lines below
#define addressChange 8 //the distance between addresses for our eeprom
//Task Control Blocks
TCB measurementTCB;         // Declare all TCBs
TCB touchScreenTCB;
TCB SOCTCB;
TCB contactorTCB;
TCB alarmTCB;
TCB remoteTerminalTCB;
TCB dataLoggingTCB;



// all data
measurementData measure;    // Declare measurement data structure - defined in Measurement.h
touchScreenData touch;      // Declare touch screen data structure - defined in TouchScreen.h
SOCData SOC;                // Declare SOC data structure - defined in SOC.h
contactorData contactor;   // Declare contractor data structure - defined in contactor.h
alarmData alarm;            // Declare alarm data structure - defined in Alarm.h
remoteTerminalData remoteTerminal; //Declare remote terminal data structure - defined in RemoteTerminal.h
dataLoggingData dataLogging; //Declare data logging data structure - defined in DataLogging.h

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

//multiple uses global variables
int counter = 1;
bool batteryOn = false; //true = ON false = OFF
const byte batteryPin = 40;
volatile bool timeBaseFlag = false;
volatile bool alarmReset = false; //this is the check to turn all alarm flags off 
volatile bool resetEEPROM = false; 

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

  //initializes timerOne flag
  Timer1.initialize(100E+3);        //set the timer period to 100ms
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
  
  
  

  attachInterrupt(digitalPinToInterrupt(hvilPin), alarmISR, RISING);  //creates an interrupt whenever hvilPin is high
  
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

  //creating the doubly linked list
  measurementTCB.prev = NULL;
  measurementTCB.next = &SOCTCB;
  SOCTCB.prev = &measurementTCB;
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
