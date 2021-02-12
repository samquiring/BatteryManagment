#include "Measurement.h"
#include "TaskControlBlock.h"
#define NUM_TASKS 5

//TODO: ADD UPDATE COUNTER AFTER EVERY CYCLE
void scheduler(TCB* taskArray[]){
     /****************
    * Function name: scheduler
    * Function inputs: TCB* curTask
    * Function outputs: void
    * Function description: calls all tasks in given linked list
    * Author(s): Sam Quiring
    *****************/
    /*
    while(*curTask != null){
      *curTask.task((*curTask).taskDataPtr)
      *curTask = *curTask.next;
    }
    */
    int i;
    for(i = 0; i < NUM_TASKS; i++){
      (*taskArray[i]).task((*taskArray[i]).taskDataPtr);
    }
}
