#include "Measurement.h"
#include "TaskControlBlock.h"
#include <Arduino.h>
#define NUM_TASKS 6
#define hz_10 10 //the amount of cycles to skip to reach this frequency
#define hz_1 100 //the amount of cycles to skip to reach this frequency


void insert(TCB* curTask, TCB* taskArray[], int index){
  while(curTask->next != NULL){
    curTask = curTask->next;
  }
  curTask->next = taskArray[index];
  taskArray[index]->next = NULL;
  taskArray[index]->prev = curTask;

}


void Delete(TCB* curTask, TCB* taskArray[], int ArrayIndex){
  if(curTask){
    if(curTask->named == taskArray[ArrayIndex]->named){
      curTask = curTask->next;
      curTask->prev = NULL;
    } else {
      while(curTask && curTask->next && curTask->next->named != taskArray[ArrayIndex]->named){
        curTask = curTask->next;
      }
      if(curTask->next->named == taskArray[ArrayIndex]->named){
        curTask->next = curTask->next->next;
        if(curTask->next){
            curTask->next->prev = curTask;
        }
        taskArray[ArrayIndex]->next = NULL;
        taskArray[ArrayIndex]->prev = NULL;
      }
    }
  }
}


long scheduler(TCB* curTask, TCB* taskArray[], int* counter){
     /****************
    * Function name: scheduler
    * Function inputs: TCB* curTask
    * Function outputs: void
    * Function description: calls all tasks in given linked list
    * Author(s): Sam Quiring
    *****************/
    unsigned long holderTime = 0;
    if(*counter % hz_10 == 1){
      Delete(curTask, taskArray,3);
      Delete(curTask, taskArray,4);
      if(*counter != 0){
        insert(curTask, taskArray,5);
        insert(curTask, taskArray,6);
      }
    }
    if(*counter % hz_1 == 1){
      Delete(curTask, taskArray, 0);
      Delete(curTask, taskArray, 1);
      if(*counter % hz_1*5 == 1){
        Delete(curTask, taskArray, 2);
      }
    }

    if(*counter % hz_10 == 0){
      Delete(curTask, taskArray,5);
      Delete(curTask, taskArray,6);
      if(*counter != 0){
        insert(curTask, taskArray,3);
        insert(curTask, taskArray,4);
      }
    }
    if(*counter % hz_1 == 0 && counter != 0){
      if(*counter % hz_1*5 == 0){
          insert(curTask, taskArray,2);
        }
      insert(curTask, taskArray,0);
      insert(curTask, taskArray,1);
      }
     holderTime = millis();
    while(curTask){
      curTask->task((curTask->taskDataPtr));
      curTask = curTask->next;
      
    }
    return(millis()-holderTime);
}
