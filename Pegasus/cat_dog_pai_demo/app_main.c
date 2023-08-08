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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_i2c.h"
#include "iot_gpio.h"
#include "iot_gpio_ex.h"
#include "iot_adc.h"
#include "iot_uart.h"
#include "iot_errno.h"
#include "hi_task.h"
#include "hi_uart.h"

#include "oled_ssd1306.h"
#include "app_sg90.h"
#include "app_hx711.h"
#include "app_hcsr.h"
#include "app_uart.h"


#define AHT20_BAUDRATE (400 * 1000)
#define AHT20_I2C_IDX 0

#define STACK_SIZE 4096

static void Hx711Demo(void)
{
    osThreadAttr_t attr;
    attr.name = "hx711_task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 4096; 
    attr.priority = osPriorityNormal;

    if (osThreadNew(hi_hx711_task, NULL, &attr) == NULL) {
        printf("[Hx711Demo] Falied to create hx711_task!\n");
    }
}

APP_FEATURE_INIT(Hx711Demo);

//串口收发任务
/*
 * This demo simply shows how to read datas from UART2 port and then echo back.
 */
hi_void UartTransmit(hi_void)
{
    hi_u32 ret = 0;

    IotUartAttribute uartAttr = {
        .baudRate = 115200, /* baudRate: 115200 */
        .dataBits = 8, /* dataBits: 8bits */
        .stopBits = 1, /* stop bit */
        .parity = 0,
    };
    /* Initialize uart driver */
    //ret = IoTUartInit(DEMO_UART_NUM, &uartAttr);
    ret = IoTUartInit(DEMO_UART_NUM, &uartAttr);
    if (ret != HI_ERR_SUCCESS) {
        printf("Failed to init uart! Err code = %d\n", ret);
        return;
    }
    /* Create a task to handle uart communication */
    osThreadAttr_t attr = {0};
    attr.stack_size = 4096;
    attr.priority = osPriorityNormal;
    attr.name = (hi_char*)"uart demo";
    OledInit();
    OledFillScreen(0);
    if (osThreadNew((osThreadFunc_t)Uart_TX, NULL, &attr) == NULL) {
        printf("Falied to create uartTX demo task!\n");
    }
}
APP_FEATURE_INIT(UartTransmit);

