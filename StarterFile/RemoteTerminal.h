#ifndef REMOTETERMINAL_H_INCLUDED
#define REMOTETERMINAL_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif
typedef struct remoteTerminalTaskData {
    float* temperature;
    float* hvCurrent;
    float* hvVoltage;
    volatile bool* remoteTerminalFlag; //true = run task false = skip task
    bool* resetFlag;
    bool* runStartFunct; //true if we need to re print the startup values
} remoteTerminalData;


void dataLoggingTask (void*);


#endif

#ifdef __cplusplus
}
#endif
