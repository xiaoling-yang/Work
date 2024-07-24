#ifndef  __MYCALLBACKFUNC_H
#define  __MYCALLBACKFUNC_H
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "AS608.h"
#include "adc.h"
#include "syn6288.h"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);//外部中断回调处理函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);//定时器中断回调函数
void Car_Status(uint8_t status);//车辆控制 1 启动车辆 0关闭车辆
void remove_newline(char *str);

#endif
