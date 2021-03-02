#include "Measurement.h"
#include "TaskControlBlock.h"
#define NUM_TASKS 5

void insert(TCB* curTask, TCB* taskArray[], int ArrayIndex){
  curTask->next = taskArray[ArrayIndex];
  taskArray[ArrayIndex]->next = NULL;
  taskArray[ArrayIndex]->prev = curTask;
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
        curTask->next->prev = curTask;
        taskArray[ArrayIndex]->next = NULL;
        taskArray[ArrayIndex]->prev = NULL;
        
      }
    }
  }
}


void scheduler(TCB* curTask, TCB* taskArray[], int* counter, int runthrough[]){
     /****************
    * Function name: scheduler
    * Function inputs: TCB* curTask
    * Function outputs: void
    * Function description: calls all tasks in given linked list
    * Author(s): Sam Quiring
    *****************/
    int x = 0;
    if(*counter % 10 == 1){
      //Delete(curTask, taskArray, 0);
     // Delete(curTask, taskArray, 1);
      if(*counter % 50 == 1){
        //Delete(curTask, taskArray, 2);
      }
    }
    while(curTask){
      runthrough[x] = curTask->named;
      x++;
      curTask->task((curTask->taskDataPtr));
      curTask = curTask->next;
      if(*counter % 10 == 0 && !curTask->next){
        //insert(curTask, taskArray,0);
        //insert(curTask, taskArray,1);
        if(*counter % 50 == 0){
          //insert(curTask, taskArray,2);
        }
      }
    }
}
