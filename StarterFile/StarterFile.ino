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
#include <TimerOne.h>

#define CHARSIZE 60 //size of the lines below
//Task Control Blocks
TCB measurementTCB;         // Declare all TCBs
TCB touchScreenTCB;
TCB SOCTCB;
TCB contactorTCB;
TCB alarmTCB;



// all data
measurementData measure;    // Declare measurement data structure - defined in Measurement.h
touchScreenData touch;      // Declare touch screen data structure - defined in TouchScreen.h
SOCData SOC;                // Declare SOC data structure - defined in SOC.h
contactorData contactor;   // Declare contractor data structure - defined in contactor.h
alarmData alarm;            // Declare alarm data structure - defined in Alarm.h

//measure global variables
//start with the number that was in their first state
float hvCurrent     = 0.0;
float hvVoltage     = 10.0;
float temperature   = 0.0;
bool HVIL           = false;
<<<<<<< HEAD
const byte hvilPin = 20; //the current state of pin 22. false = low true = high

int hvVoltagePin = 0;

=======
<<<<<<< HEAD
const byte hvilPin = 22; //the current state of pin 22. false = low true = high
int tempPin = 11;
int voltagePin =12;
int currentPin = 13;
=======
const byte hvilPin = 20; //the current state of pin 22. false = low true = high
>>>>>>> 29b47b50d0cd8c365a0ada1e0575803c87dc898e
>>>>>>> 48548884b8e2ded5554569de8270a52398ed2469
float chargeState = 0;
volatile bool measurementFlag = true;

//contactor global variables
bool contactorState = true; //true = open, false = closed
volatile bool contactorFlag = true;

//Alarm global variables
int HVILState = 0;
int OvercurrentState = 0;
int HVOutOfRangeState = 0;
volatile bool alarmFlag = true;

//SOC global variables
int stateOfCharge = 0;
volatile bool SOCFlag = true;

//touchscreen global variables
bool initialize = true; 
int touchState = 0; //0 = measurement, 1 = battery, 2 = Alarm | Battery screen is seperate
bool thingsChanged = true;
char oneLine[CHARSIZE];
char twoLine[CHARSIZE];
char threeLine[CHARSIZE];
char fourLine[CHARSIZE];
char fiveLine[CHARSIZE];
bool updateStates = true;
volatile bool touchScreenFlag = true;
volatile bool forceAlarm = false; //sets when the touchscreen is stuck in the alarmState

//multiple uses global variables
int counter = 1;
bool batteryOn = false; //true = ON false = OFF
const byte batteryPin = 40;
volatile bool timeBaseFlag = false;

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

<<<<<<< HEAD
  //pullup resistor and pin for HVIL voltage
  pinMode(A15, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(hvilPin), alarmISR, RISING);  //creates an interrupt whenever hvilPin is high
=======
<<<<<<< HEAD
  // Anders attempt to set up pins

  pinMode(tempPin, INPUT);
  pinMode(voltagePin, INPUT);
  pinMode(currentPin, INPUT);
=======
  attachInterrupt(digitalPinToInterrupt(hvilPin), alarmISR, RISING);  //creates an interrupt whenever hvilPin is high
>>>>>>> 29b47b50d0cd8c365a0ada1e0575803c87dc898e
>>>>>>> 48548884b8e2ded5554569de8270a52398ed2469
  
  //setting all variables up so they are used in functions
  measure.hvilStatus = &HVIL;
  measure.hvilPin = &hvilPin;
  measure.temperature = &temperature;
  measure.hvCurrent = &hvCurrent;
  measure.hvVoltage = &hvVoltage;
  measure.counter = &counter;
  measure.measurementFlag = &measurementFlag;

  contactor.contactorState = &contactorState;
  contactor.batteryOn = &batteryOn;
  contactor.contactorFlag = &contactorFlag;

  alarm.HVILState = &HVILState;
  alarm.OvercurrentState = &OvercurrentState;
  alarm.HVOutOfRangeState = &HVOutOfRangeState;
  alarm.counter = &counter;
  alarm.alarmFlag = &alarmFlag;
  alarm.forceAlarm = &forceAlarm;

  SOC.stateOfCharge = &stateOfCharge;
  SOC.SOCFlag = &SOCFlag;

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

  //setting TCB up so it is connected
  measurementTCB.taskDataPtr = &measure;
  measurementTCB.task = &measurementTask;

  contactorTCB.taskDataPtr = &contactor;
  contactorTCB.task = &contactorTask;

  alarmTCB.taskDataPtr = &alarm;
  alarmTCB.task = &alarmTask;

  SOCTCB.taskDataPtr = &SOC;
  SOCTCB.task = &SOCTask;

  touchScreenTCB.taskDataPtr = &touch;
  touchScreenTCB.task = &touchScreenTask;

  //Initialize serial communication
    Serial.begin(9600);
    Serial1.begin(9600);
    Serial1.setTimeout(1000);

}
//creating an array of all TCBs so that scheduler can parse through them
TCB* taskArray[] = {&touchScreenTCB,&measurementTCB,&SOCTCB,&contactorTCB,&alarmTCB};
void loop() {
  /****************
    * Function name:    loop
    * Function inputs:  Sensor data, touch input
    * Function outputs: Display data and lights indicating alarm status, contactor status, sensor data, & state of charge
    * Function description: This is a round robin scheduler to run a series of tasks
    *                       that create a user interface to a battery management system
    * Author(s): Sam Quiring
    *****************/
    while(1){
      if(timeBaseFlag){
           timeBaseFlag = false;
           scheduler(taskArray);
           digitalWrite(batteryPin,batteryOn);  //might need to put this inside of battery function
           digitalWrite(A15,hvVoltagePin);
           Serial.print(" " + hvVoltagePin);
           counter++;
      }
    }
   
   ///counter += 1; I don't think we need these anymore
    //delay(930);   TODO: figure out if these matter

   
}

void timerISR(){          //interrupts service routine
  noInterrupts();
  timeBaseFlag = true;   //set timerISR flag
  interrupts();
}
void alarmISR(){
  noInterrupts();
  measurementFlag = false;
  SOCFlag = false;
  contactorFlag = false;
  Serial.println("Alarm Flag Raised");
  interrupts();
}
