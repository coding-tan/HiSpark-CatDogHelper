#ifndef __APP_HX711_H__
#define __APP_HX711_H__
#include <hi_stdlib.h>
#include "hi_io.h"
#include "hi_gpio.h"

hi_void *hi_hx711_task(hi_void* param);
unsigned long Sensor_Read(hi_gpio_idx id1, hi_gpio_idx id2);
double Get_Sensor_Read(hi_gpio_idx id1, hi_gpio_idx id2);

#endif

