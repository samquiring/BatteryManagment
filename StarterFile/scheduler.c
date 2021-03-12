#include "Measurement.h"
#include "TaskControlBlock.h"
#include <Arduino.h>
#define NUM_TASKS 5

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
    if(*counter % 10 == 1){
      //Delete(curTask, taskArray, 0);
      Delete(curTask, taskArray, 1);
      if(*counter % 50 == 1){
        Delete(curTask, taskArray, 2);
      }
    }

    if(*counter % 10 == 0){
      if(*counter % 50 == 0){
          insert(curTask, taskArray,2);
        }
    //insert(curTask, taskArray,0);
      insert(curTask, taskArray,1);
      }
     holderTime = millis();
    while(curTask){
      curTask->task((curTask->taskDataPtr));
      curTask = curTask->next;
      
    }
    return(millis()-holderTime);
}
