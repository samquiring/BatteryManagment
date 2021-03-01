#ifndef DATALOGGING_H_INCLUDED
#define DATALOGGING_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif
typedef struct dataLoggingTaskData {
    float* temperature;
    bool* tempChange;
    int* tempAdress;
    float* hvCurrent;
    bool* currentChange;
    int* currentAddress;
    float* hvVoltage;
    bool* voltageChange;
    int* voltageAddress;
    volatile bool* dataLoggingFlag; //true = run task false = skip task
} dataLoggingData;


void dataLoggingTask (void*);


#endif

#ifdef __cplusplus
}
#endif
