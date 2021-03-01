#ifndef REMOTETERMINAL_H_INCLUDED
#define REMOTETERMINAL_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif
typedef struct remoteTerminalTaskData {
    float* temperatureMin;
    float* temperatureMax;
    float* currentMin;
    float* currentMax;
    float* voltageMin;
    float* voltageMax;
    volatile bool* remoteTerminalFlag; //true = run task false = skip task
    volatile bool* resetFlag;
    bool* runStartFunct; //true if we need to re print the startup values
    int* incomingByte;
} remoteTerminalData;


void remoteTerminalTask (void*);


#endif

#ifdef __cplusplus
}
#endif
