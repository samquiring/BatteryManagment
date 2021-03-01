#ifndef DATALOGGING_H_INCLUDED
#define DATALOGGING_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif
typedef struct dataLoggingTaskData {
    bool* tempChangeMin;
    bool* tempChangeMax;
    int* tempAddressMin;
    int* tempAddressMax;
    bool* currentChangeMin;
    bool* currentChangeMax;
    int* currentAddressMin;
    int* currentAddressMax;
    bool* voltageChangeMin;
    bool* voltageChangeMax;
    int* voltageAddressMin;
    int* voltageAddressMax;
    float* currentMin;
    float* currentMax;
    float* voltageMin;
    float* voltageMax;
    float* temperatureMin;
    float* temperatureMax;
    volatile bool* dataLoggingFlag; //true = run task false = skip task
    volatile bool* resetEEPROM;
} dataLoggingData;


void dataLoggingTask (void*);


#endif

#ifdef __cplusplus
}
#endif
