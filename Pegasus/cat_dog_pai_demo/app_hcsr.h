#ifndef __APP_HCSR_H__
#define __APP_HCSR_H__

enum HcsrState{
    Hcsr_OFF,
    Hcsr_ON,    
};

void HcsrGpioInit(void);
int *HcsrTask(const char *arg);
void OnHcsr();

#endif