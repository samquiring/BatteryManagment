#ifndef REMOTETERMINAL_H_INCLUDED
#define REMOTETERMINAL_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif
typedef struct remoteTerminalTaskData {
    float* temperatureMin;
    float* temperatureMax;
    float* hvCurrentMin;
    float* hvCurrentMax;
    float* hvVoltageMin;
    float* hvVoltageMax;
    volatile bool* remoteTerminalFlag; //true = run task false = skip task
    bool* resetFlag;
    bool* runStartFunct; //true if we need to re print the startup values
} remoteTerminalData;


void remoteTerminalTask (void*);


#endif

#ifdef __cplusplus
}
#endif
