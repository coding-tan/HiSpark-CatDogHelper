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
 
#include <hi_stdlib.h>
#include <hi_uart.h>
#include <iot_uart.h>
#include <hi_gpio.h>
#include <hi_io.h>
#include "iot_i2c.h"
#include "iot_gpio_ex.h"
#include "ohos_init.h"
#include "iot_gpio.h"
#include "cmsis_os2.h"
#include "oled_ssd1306.h"
#include "app_uart.h"
#include "oled_fonts.h"
#include "hisignalling_protocol.h"
#include "app_sg90.h"
#include "app_hcsr.h"
#include "app_hx711.h"

#define LED_INTERVAL_TIME_US 100000
#define AHT20_BAUDRATE (400 * 1000)
#define AHT20_I2C_IDX 0

extern g_HcsrState;
extern double hx711_weight; //食槽中猫粮重量
extern double hx711_weight_2; //食槽中狗粮重量

UartDefConfig uartDefConfig = {0};

static void Uart1GpioCOnfig(void)    //定义哪个板子
{
#ifdef ROBOT_BOARD
    IoSetFunc(HI_IO_NAME_GPIO_5, IOT_IO_FUNC_GPIO_5_UART1_RXD);
    IoSetFunc(HI_IO_NAME_GPIO_6, IOT_IO_FUNC_GPIO_6_UART1_TXD);
    /* IOT_BOARD */
#elif defined (EXPANSION_BOARD)
    IoSetFunc(HI_IO_NAME_GPIO_0, IOT_IO_FUNC_GPIO_0_UART1_TXD);
    IoSetFunc(HI_IO_NAME_GPIO_1, IOT_IO_FUNC_GPIO_1_UART1_RXD);
#endif
}

int SetUartRecvFlag(UartRecvDef def)   //标志位
{
    if (def == UART_RECV_TRUE) {
        uartDefConfig.g_uartReceiveFlag = HI_TRUE;
    } else {
        uartDefConfig.g_uartReceiveFlag = HI_FALSE;
    }
    
    return uartDefConfig.g_uartReceiveFlag;
}

int GetUartConfig(UartDefType type)    //串口配置
{
    int receive = 0;

    switch (type) {
        case UART_RECEIVE_FLAG:
            receive = uartDefConfig.g_uartReceiveFlag;
            break;
        case UART_RECVIVE_LEN:
            receive = uartDefConfig.g_uartLen;
            break;
        default:
            break;
    }
    return receive;
}

void ResetUartReceiveMsg(void)   //重置串口接收到的数据
{
    (void)memset_s(uartDefConfig.g_receiveUartBuff, sizeof(uartDefConfig.g_receiveUartBuff),
        0x0, sizeof(uartDefConfig.g_receiveUartBuff));
}

unsigned char *GetUartReceiveMsg(void)    //获取串口接收的数据
{
    return uartDefConfig.g_receiveUartBuff;
}

static const char *data = "0xfe";   //有遮挡即有动物靠近    
static const char *data1 = "0xff";  //无遮挡即无动物靠近

// static char data = 0x01;
// static char data1 = 0x02;

uint8_t uartBuff[UART_BUFF_SIZE] = { 0 };
uint8_t *uart_buff_ptr = uartBuff;

char *flag = "none";
int g_CatUartFlag = 0;
int g_DogUartFlag = 0;
void Uart_TX(void)
{
    uint8_t ret;
    HcsrGpioInit();
    printf("Hcsr is initialized!\r\n");
    Uart1GpioCOnfig(); //配置串口IO口
    printf("Initialize uart demo successfully, please enter some datas via DEMO_UART_NUM port...\n");
    while(1)
    {
        OnHcsr();
        switch (g_HcsrState)
        {
        case Hcsr_ON: //有动物靠近
            printf("Hcsr is on!");
            IoTUartWrite(DEMO_UART_NUM, (unsigned char *)data, strlen(data));
            IoTUartRead(DEMO_UART_NUM, uart_buff_ptr, UART_BUFF_SIZE);
            printf("uartBuff_on[3] = %c\r\n", uartBuff[3]);
            //printf("Uart1 read data:%s\n", uart_buff_ptr);
            break;
        case Hcsr_OFF: //无动物靠近
            printf("Hcsr is off!");
            IoTUartWrite(DEMO_UART_NUM, (unsigned char *)data1, strlen(data));
            IoTUartRead(DEMO_UART_NUM, uart_buff_ptr, UART_BUFF_SIZE);
            printf("uartBuff_off[3] = %c\r\n", uartBuff[3]);
            //printf("Uart1 read data:%s\n", uart_buff_ptr);
            break;
        default:
            break;
        }
        
        if(uartBuff[3]!=0)
        {
            IoTI2cInit(AHT20_I2C_IDX, AHT20_BAUDRATE);
        }
        switch (uartBuff[3])
        {
        case '1': //猫
            OledShowString(20, 3, "   cat  ", 1); /* 屏幕第20列3行显示1行 */
            printf("cat is coming!\r\n");
            // if(hx711_weight <= 25)
            // {
            //     open_cat_motor();
            //     close_cat_motor();
            // }
            
            break;
        case '2': //狗
            OledShowString(20, 3, "   dog  ", 1); /* 屏幕第20列3行显示1行 */
            printf("dog is coming!\r\n");
            // if (hx711_weight_2 <= 25)
            // {
            //     open_dog_motor();
            //     close_dog_motor();
            // }
            break;
        // case '4': //健康狗
        //     OledShowString(20, 3, "rhino   ", 1); /* 屏幕第20列3行显示1行 */
        //     IoTGpioSetOutputVal(LED_TEST_GPIO, 1);
        //     usleep(LED_INTERVAL_TIME_US);
        //     IoTGpioSetOutputVal(LED_TEST_GPIO, 0);
        //     usleep(LED_INTERVAL_TIME_US);
        //     break;
        // case '8': //非健康狗
        //     OledShowString(20, 3, "zebra   ", 1); /* 屏幕第20列3行显示1行 */
        //     IoTGpioSetOutputVal(LED_TEST_GPIO, 1);
        //     usleep(LED_INTERVAL_TIME_US);
        //     IoTGpioSetOutputVal(LED_TEST_GPIO, 0);
        //     usleep(LED_INTERVAL_TIME_US);
        //     break;

        default:
            OledShowString(20, 3, "waiting ", 1); /* 屏幕第20列3行显示1行 */
            break;
        }
        hi_sleep(500); /* 延时500ms */
    }
}

// 创建识别后喂食任务, we create a task here
static void *UartFeedingTask(void)
{
    while (1)
    {
        printf("adadawedadad\r\n");
        printf("uartBuff[3] = %d\r\n", uartBuff[3]);
        if (uartBuff[3] == '1' && hx711_weight <= 50)
        {
            open_cat_motor();
            close_cat_motor();
        }
        else
        {
            g_CatUartFlag = 0;
        }

        if (uartBuff[3] == '2' && hx711_weight_2 <= 50)
        {
            open_dog_motor();
            close_dog_motor();
        }
        else
        {
            g_DogUartFlag = 0;
        }
        printf("uartBuff[3]_feeding = %d\r\n", uartBuff[3]);
        hi_sleep(200);
    }
}

static void UartFeedDemo(void)
{
    osThreadAttr_t attr;
    attr.name = "uart_feed_task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 4096; 
    attr.priority = osPriorityNormal;

    if (osThreadNew(UartFeedingTask, NULL, &attr) == NULL) {
        printf("[UartFeedCatDemo] Falied to create uart_feed_cat_task!\n");
    }
}

APP_FEATURE_INIT(UartFeedDemo);