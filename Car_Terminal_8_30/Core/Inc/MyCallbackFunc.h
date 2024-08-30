#ifndef  __MYCALLBACKFUNC_H
#define  __MYCALLBACKFUNC_H
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "AS608.h"
#include "adc.h"
#include "syn6288.h"

#define LED_FP              0
#define LED_Alcohol         1
#define LED_Error           2
#define LED_Ready           3
#define LED_JiuJinChaoBiao  4
#define LED_PiNaoJiaShi     5
#define LED_OverRange       6
#define LED_Storage         7




void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);//外部中断回调处理函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);//定时器中断回调函数
void Car_Status(uint8_t status);//车辆控制 1 启动车辆 0关闭车辆
void remove_newline(char *str);//去除换行符
void LED_FastFlush(GPIO_TypeDef * PORT_TYPE,uint16_t PORT_PIN);//LED快闪
void LED_SlowFlush(GPIO_TypeDef * PORT_TYPE,uint16_t PORT_PIN);//LED慢闪
void LED_Constantlight(GPIO_TypeDef * PORT_TYPE,uint16_t PORT_PIN);//LED常亮
void LED_Off(GPIO_TypeDef * PORT_TYPE,uint16_t PORT_PIN);//LED关闭
void LED_Control(uint8_t LedType,uint8_t ways);//0关闭1打开2慢闪3快闪

#endif
