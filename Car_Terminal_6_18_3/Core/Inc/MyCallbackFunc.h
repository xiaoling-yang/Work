#ifndef  __MYCALLBACKFUNC_H
#define  __MYCALLBACKFUNC_H
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "AS608.h"
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);//外部中断回调处理函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);//定时器中断回调函数
#endif
