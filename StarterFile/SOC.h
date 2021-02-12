
#ifdef __cplusplus
extern "C" {
#endif

#ifndef SOC_H_
#define SOC_H_


#define HVIL_OPEN   false
#define HVIL_CLOSED true

#include <stdlib.h>
#include <stdbool.h>


typedef struct SOCTaskData {
  int* stateOfCharge;
  volatile bool* SOCFlag; //true = run task false = skip task
} SOCData;


void SOCTask (void*);


#endif

#ifdef __cplusplus
}
#endif
