/*
 * Copyright (C) 2022 HiHope Open Source Organization .
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
 *
 * limitations under the License.
  */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "hi_io.h"
#include "hi_time.h"

#define GPIO06 6  //舵机1 GPIO　06 控制舵机猫粮
#define GPIO09 9  //舵机3 GPIO　09 控制舵机狗粮

#define COUNT 40
void set_angle(unsigned int duty, unsigned int gpio)
{
    unsigned int time = 20000;
    IoTGpioInit(gpio);
    IoTGpioSetDir(gpio, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(gpio, IOT_GPIO_VALUE1);
    hi_udelay(duty);
    IoTGpioSetOutputVal(gpio, IOT_GPIO_VALUE0);
    hi_udelay(time - duty);
}

/* 打开控制舵机猫粮*/
void open_cat_motor(void)
{
    unsigned int angle = 500; //舵机向左旋转90°
    for (int i = 0; i < COUNT; i++) 
    {
        set_angle(500, GPIO06);
    }

}

/* 关闭控制舵机猫粮 */
void close_cat_motor(void)
{
    unsigned int angle = 2500; //舵机向右旋转90° 
    for (int i = 0; i < COUNT; i++) 
    {
        set_angle(2500, GPIO06);
    }
}



/* 打开控制舵机狗粮 */
void open_dog_motor(void)
{
    unsigned int angle = 500; //舵机向左旋转90°
    for (int i = 0; i < COUNT; i++) 
    {
        set_angle(500, GPIO09);
    }
}

/* 关闭控制舵机狗粮*/
void close_dog_motor(void)
{
    unsigned int angle = 2500; //舵机向右旋转90°
    for (int i = 0; i < COUNT; i++) 
    {
        set_angle(2500, GPIO09);
    }
}
