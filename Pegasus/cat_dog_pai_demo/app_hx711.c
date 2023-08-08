#include <math.h>
#include <hi_early_debug.h>
#include <hi_time.h>
#include <hi_adc.h>
#include <hi_stdlib.h>
#include <hi_watchdog.h>
#include <hi_pwm.h>
#include <hi_io.h>
#include <hi_gpio.h>


//校准参数
//因为不同的传感器特性曲线不是很一致，因此，每一个传感器需要矫正这里这个参数才能使测量值很准确。
//当发现测试出来的重量偏大时，增加该数值。
//如果测试出来的重量偏小时，减小改数值。
//该值可以为小数,可参考误差1g调整10

#define GapValue 495
#define Hx711_DT 11 //定义GPIO11为GPIO输入，为传感器DT引脚
#define Hx711_SCK 12 //定义GPIO12为GPIO输出，为传感器SCK引脚
#define Hx711_DT_2 8
#define Hx711_SCK_2 2

unsigned long Sensor_Read_Cat(void);
double Get_Sensor_Read_Cat(void);

unsigned long Sensor_Read(hi_gpio_idx id1, hi_gpio_idx id2);
double Get_Sensor_Read(hi_gpio_idx id1, hi_gpio_idx id2);

double hx711_weight = 0; 
double hx711_weight_2 = 0;
hi_void *hi_hx711_task(void)
{	
	//初始化GPIO11为GPIO输入，为传感器DT引脚
    hi_io_set_func(HI_IO_NAME_GPIO_11, HI_IO_FUNC_GPIO_11_GPIO);
    hi_gpio_set_dir(HI_GPIO_IDX_11,HI_GPIO_DIR_IN);
	//初始化GPIO12为GPIO输出，为传感器SCK引脚
    hi_io_set_func(HI_IO_NAME_GPIO_12, HI_IO_FUNC_GPIO_12_GPIO);
    hi_gpio_set_dir(HI_GPIO_IDX_12,HI_GPIO_DIR_OUT);
    hi_gpio_set_ouput_val(HI_IO_NAME_GPIO_12,0);

	//初始化GPIO8为GPIO输入，为传感器DT引脚
    hi_io_set_func(HI_IO_NAME_GPIO_8, HI_IO_FUNC_GPIO_8_GPIO);
    hi_gpio_set_dir(HI_GPIO_IDX_8,HI_GPIO_DIR_IN);
	//初始化GPIO2为GPIO输出，为传感器SCK引脚
    hi_io_set_func(HI_IO_NAME_GPIO_2, HI_IO_FUNC_GPIO_2_GPIO);
    hi_gpio_set_dir(HI_GPIO_IDX_2,HI_GPIO_DIR_OUT);
    hi_gpio_set_ouput_val(HI_IO_NAME_GPIO_2,0);

	double base_data = 0;
    double base_data1 = 0;
	hi_u8 buf[10] = {0};

	base_data = Get_Sensor_Read(Hx711_DT_2,Hx711_SCK_2); //获取基准值
    base_data1 = Get_Sensor_Read(Hx711_DT,Hx711_SCK); //获取基准值
     while (1)
     {
		hx711_weight = (Get_Sensor_Read(Hx711_DT_2,Hx711_SCK_2) - base_data) / GapValue; //获取重量

        if(hx711_weight > 0) // 大于0时显示
        {
            printf("Cat Food:   %.2f\r\n" ,hx711_weight);  
        }

        hx711_weight_2 = (Get_Sensor_Read(Hx711_DT,Hx711_SCK) - base_data1) / GapValue; //获取重量

        if(hx711_weight_2 > 0) // 大于0时显示
        {
            printf("Dog Food:   %.2f\r\n" ,hx711_weight_2);  
        }
		IotPublishSample(hx711_weight, hx711_weight_2); // 发布例程   
		hi_sleep(1000); 
     }
}


unsigned long Sensor_Read(hi_gpio_idx id1, hi_gpio_idx id2)
{
	unsigned long value = 0;
	unsigned char i = 0;
	hi_gpio_value input = 0;
	hi_udelay(2);
	//时钟线拉低 空闲时时钟线保持低电位
	hi_gpio_set_ouput_val(id1,0);
	hi_udelay(2);	
	hi_gpio_get_input_val(id1,&input);
	//等待AD转换结束
	while(input)
    {
        hi_gpio_get_input_val(id1,&input);
    }
	for(i=0;i<24;i++)
	{
		//时钟线拉高 开始发送时钟脉冲
		hi_gpio_set_ouput_val(id2,1);
		hi_udelay(2);
		//左移位 右侧补零 等待接收数据
		value = value << 1;
		//时钟线拉低
		hi_gpio_set_ouput_val(id2,0);
		hi_udelay(2);
		//读取一位数据
        hi_gpio_get_input_val(id1,&input);
		if(input){
			value ++;
        }
	}
	//第25个脉冲
	hi_gpio_set_ouput_val(id2,1);
	hi_udelay(2);
	value = value^0x800000;	
	//第25个脉冲结束
	hi_gpio_set_ouput_val(id2,0);	
	hi_udelay(2);	
	return value;
}

double Get_Sensor_Read(hi_gpio_idx id1, hi_gpio_idx id2)
{
  	double sum = 0;    // 为了减小误差，一次取出10个值后求平均值。
  	for (int i = 0; i < 2; i++) // 循环的越多精度越高，当然耗费的时间也越多
		sum += Sensor_Read(id1,id2);  // 累加
  	return (sum/2); // 求平均值进行均差

}
