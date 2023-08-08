/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <hi_task.h>
#include <string.h>
#include <hi_wifi_api.h>
#include <hi_mux.h>
#include <hi_io.h>
#include <hi_gpio.h>
#include "iot_config.h"
#include "iot_log.h"
#include "iot_main.h"
#include "iot_profile.h"
#include "ohos_init.h"
#include "cmsis_os2.h"

/* attribute initiative to report */
#define TAKE_THE_INITIATIVE_TO_REPORT
#define ONE_SECOND                          (1000)
/* oc request id */
#define CN_COMMADN_INDEX                    "commands/request_id="
#define WECHAT_SUBSCRIBE_Motor_Cat              "motor_cat"
#define WECHAT_SUBSCRIBE_Motor_Cat_ON_STATE     "1"
#define WECHAT_SUBSCRIBE_Motor_Cat_OFF_STATE    "0"
#define WECHAT_SUBSCRIBE_Motor_Dog              "motor_dog"
#define WECHAT_SUBSCRIBE_Motor_Dog_ON_STATE     "1"
#define WECHAT_SUBSCRIBE_Motor_Dog_OFF_STATE    "0"

extern double hx711_weight; //食槽中猫粮重量
extern double hx711_weight_2; //食槽中狗粮重量
double hx711_weight_total = 1000; //总的猫粮量
double hx711_weight_total_2 = 1000; //总的狗粮量

int g_motor_catStatus = -1;
int g_motor_dogStatus = -1;
int g_CatCallbackFlag = 0;
int g_DogCallbackFlag = 0;

typedef void (*FnMsgCallBack)(hi_gpio_value val);

typedef struct FunctionCallback {
    hi_bool  stop;
    hi_u32 conLost;
    hi_u32 queueID;
    hi_u32 iotTaskID;
    FnMsgCallBack    msgCallBack;
}FunctionCallback;
FunctionCallback g_functinoCallback;


static void DeviceConfigInit(hi_gpio_value val)
{
    hi_io_set_func(HI_IO_NAME_GPIO_9, HI_IO_FUNC_GPIO_9_GPIO);
    hi_gpio_set_dir(HI_GPIO_IDX_9, HI_GPIO_DIR_OUT);
    hi_gpio_set_ouput_val(HI_GPIO_IDX_9, val);
}

static int  DeviceMsgCallback(FnMsgCallBack msgCallBack)
{
    g_functinoCallback.msgCallBack = msgCallBack;
    return 0;
}

static void wechatControlDeviceMsg(hi_gpio_value val)
{
    DeviceConfigInit(val);
}

// < this is the callback function, set to the mqtt, and if any messages come, it will be called
// < The payload here is the json string
static void DemoMsgRcvCallBack(int qos, const char *topic, const char *payload)
{
    IOT_LOG_DEBUG("RCVMSG:QOS:%d TOPIC:%s PAYLOAD:%s\r\n", qos, topic, payload);
    /* 云端下发命令后，板端的操作处理 */
    if (strstr(payload, WECHAT_SUBSCRIBE_Motor_Cat) != NULL) {
        if (strstr(payload, WECHAT_SUBSCRIBE_Motor_Cat_ON_STATE) != NULL) {
                g_motor_catStatus = HI_TRUE;
                g_CatCallbackFlag = 1; 
        } 
        else {
            g_motor_catStatus = HI_FALSE;
        }
        
    }
    if (strstr(payload, WECHAT_SUBSCRIBE_Motor_Dog) != NULL){
        if (strstr(payload, WECHAT_SUBSCRIBE_Motor_Dog_ON_STATE) != NULL) {
            g_motor_dogStatus = HI_TRUE;
            g_DogCallbackFlag = 1;
        } 
        else {
            g_motor_dogStatus = HI_FALSE;
        }
    }

    return HI_NULL;
}


/* publish sample */
hi_void IotPublishSample(double hx711_weight, double hx711_weight_2)
{
    WeChatProfile weChatProfile = {
        .subscribeType = "type",
        .status.subState = "state",
        .status.subReport = "reported",
        .status.reportVersion = "version",
        .status.Token = "clientToken",
        /* report cat_food */
        .reportAction.subDeviceActionTemperature = "cat_food",
        .reportAction.temperatureData = hx711_weight_total - hx711_weight, /* 30 :cat_food data */
        /* report dog_food */
        .reportAction.subDeviceActionHumidity = "dog_food",
        .reportAction.humidityActionData = hx711_weight_total_2 - hx711_weight_2, /* dog_food data */
    };

    /* report motor_cat */
    if (g_motor_catStatus == HI_TRUE) {
        weChatProfile.reportAction.subDeviceActionMotor_cat = "motor_cat";
        weChatProfile.reportAction.motor_catActionStatus = 1; /* 1 : motor on */
    } else if (g_motor_catStatus == HI_FALSE) {
        weChatProfile.reportAction.subDeviceActionMotor_cat = "motor_cat";
        weChatProfile.reportAction.motor_catActionStatus = 0; /* 0 : motor off */
        /* report cat_dog */
    } else {
        weChatProfile.reportAction.subDeviceActionMotor_cat = "motor_cat";
        weChatProfile.reportAction.motor_catActionStatus = 0; /* 0 : motor off */
    }
    /* report motor_dog */
        if (g_motor_dogStatus == HI_TRUE) {
        weChatProfile.reportAction.subDeviceActionMotor_dog = "motor_dog";
        weChatProfile.reportAction.motor_dogActionStatus = 1; /* 1 : motor on */
    } else if (g_motor_dogStatus == HI_FALSE) {
        weChatProfile.reportAction.subDeviceActionMotor_dog = "motor_dog";
        weChatProfile.reportAction.motor_dogActionStatus = 0; /* 0 : motor off */
        /* report motor_dog */
    } else {
        weChatProfile.reportAction.subDeviceActionMotor_dog = "motor_dog";
        weChatProfile.reportAction.motor_dogActionStatus = 0; /* 0 : motor off */
    }
        
    /* profile report */
    IoTProfilePropertyReport(CONFIG_USER_ID, &weChatProfile);
}

// < this is the demo main task entry,here we will set the wifi/cjson/mqtt ready and
// < wait if any work to do in the while
static hi_void *DemoEntry(const char *arg)
{
    WifiStaReadyWait();
    cJsonInit();
    IoTMain();
    /* 云端下发回调 */
    IoTSetMsgCallback(DemoMsgRcvCallBack);
    /* 主动上报 */
    return NULL;
}

// < This is the demo entry, we create a task here,
// and all the works has been done in the demo_entry
#define CN_IOT_TASK_STACKSIZE  0x1000
#define CN_IOT_TASK_PRIOR 24
#define CN_IOT_TASK_NAME "IOTDEMO"

static void AppDemoIot(void)
{
    osThreadAttr_t attr;
    IoTWatchDogDisable();

    attr.name = "IOTDEMO";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CN_IOT_TASK_STACKSIZE;
    attr.priority = CN_IOT_TASK_PRIOR;

    if (osThreadNew((osThreadFunc_t)DemoEntry, NULL, &attr) == NULL) {
        printf("[mqtt] Falied to create IOTDEMO!\n");
    }
}

SYS_RUN(AppDemoIot);

// 创建云端控制喂食任务, we create a task here
static void *IotFeedingTask(void)
{
    while (1)
    {
        if (g_CatCallbackFlag == 1)
        {
            if (hx711_weight <= 50)
            {
                open_cat_motor();
                close_cat_motor();
            }
            else
            {
                g_CatCallbackFlag = 0;
                g_motor_catStatus = HI_FALSE;
            }

        }
        if (g_DogCallbackFlag == 1)
        {
            if (hx711_weight_2 <= 50)
            {
                open_dog_motor();
                close_dog_motor();
            }  
            else
            {
                g_DogCallbackFlag = 0;
                g_motor_dogStatus = HI_FALSE;
            }
        }
        hi_sleep(200);
    }
}

static void IoTFeedDemo(void)
{
    osThreadAttr_t attr;
    attr.name = "iot_feed_cat_task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 4096; 
    attr.priority = osPriorityNormal;

    if (osThreadNew(IotFeedingTask, NULL, &attr) == NULL) {
        printf("[IoTFeedCatDemo] Falied to create iot_feed_cat_task!\n");
    }
}

SYS_RUN(IoTFeedDemo);

// // 创建云端控制喂狗任务, we create a task here
// static void *IotDogFeedingTask(const char *arg)
// {
//     while (1)
//     {
//         if (g_DogCallbackFlag == 1)
//         {
//             if (hx711_weight_2 <= 25)
//             {
//                 open_dog_motor();
//                 close_dog_motor();
//             }  
//             else
//             {
//                 g_DogCallbackFlag = 0;
//                 g_motor_dogStatus = HI_FALSE;
//             }
//         }
//         hi_sleep(2000);
//     }
// }

// static void IoTFeedDogDemo(void)
// {
//     osThreadAttr_t attr;
//     attr.name = "iot_feed_dog_task";
//     attr.attr_bits = 0U;
//     attr.cb_mem = NULL;
//     attr.cb_size = 0U;
//     attr.stack_mem = NULL;
//     attr.stack_size = 4096; 
//     attr.priority = osPriorityNormal;

//     if (osThreadNew(IotDogFeedingTask, NULL, &attr) == NULL) {
//         printf("[IoTFeedDogDemo] Falied to create iot_feed_dog_task!\n");
//     }
// }

// SYS_RUN(IoTFeedDogDemo);