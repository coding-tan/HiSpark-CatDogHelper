#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "iot_gpio.h"
#include "iot_gpio_ex.h"
#include "app_sg90.h"
#include "app_hcsr.h"
#include <hi_gpio.h>

#define HC_GPIO 7 //设置人体红外传感器GPIO为GPIO7

enum HcsrState g_HcsrState = Hcsr_OFF;
IotGpioValue val={0};

void HcsrGpioInit()
{
    IoTGpioInit(HC_GPIO);
    IoSetFunc(HC_GPIO, 0); //设置成普通GPIO功能
    IoTGpioSetDir(HC_GPIO, IOT_GPIO_DIR_IN);
    IoSetPull(HC_GPIO, IOT_IO_PULL_DOWN);
    //IoTGpioRegisterIsrFunc(HC_GPIO, IOT_INT_TYPE_LEVEL, IOT_GPIO_EDGE_RISE_LEVEL_HIGH, OnHcsr, NULL); //中断函数
}

int *HcsrTask(const char *arg)
{
    (void)arg;
    HcsrGpioInit();
    printf("This is Hcsrtest demo\r\n");
    unsigned int time = 200;
    while (1)
    {   
        switch (g_HcsrState)
        {
        case Hcsr_ON:
            printf("hcsr is high!\n");
            open_left_feedingdoor();
            osDelay(time);
            break;
        case Hcsr_OFF:
            printf("hcsr is low!\n");
            close_left_feedingdoor();
            osDelay(time);
            break;
        default:
            osDelay(time);
            break;
        }
        
    }
    return 0;
}

void OnHcsr(void) //使用
{
    IoTGpioGetInputVal(HC_GPIO, &val);
    if (IOT_GPIO_VALUE0 == val)
    {
        g_HcsrState = Hcsr_OFF;
    }
    else
    {
        g_HcsrState = Hcsr_ON;
    }
}

// void OnHcsr(char *arg) //使用中断改变HcsrState状态
// {
//     (void)arg;
    
//     enum HcsrState nextstate = Hcsr_OFF;
//     switch (g_HcsrState)
//     {
//     case Hcsr_ON:
//         nextstate = Hcsr_OFF;
//         break;
//     case Hcsr_OFF:
//         nextstate = Hcsr_ON;
//         break;
//     default:
//         break;
//     }
//     g_HcsrState = nextstate;
// }