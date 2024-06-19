#include "MyCallbackFunc.h"

uint8_t step=1;
volatile uint8_t playSoundFlag = 0;




uint8_t SYN_StopCom[] = {0xFD, 0X00, 0X02, 0X02, 0XFD}; //停止合成
uint8_t SYN_SuspendCom[] = {0XFD, 0X00, 0X02, 0X03, 0XFC}; //暂停合成
uint8_t SYN_RecoverCom[] = {0XFD, 0X00, 0X02, 0X04, 0XFB}; //恢复合成
uint8_t SYN_ChackCom[] = {0XFD, 0X00, 0X02, 0X21, 0XDE}; //状濁查诿
uint8_t SYN_PowerDownCom[] = {0XFD, 0X00, 0X02, 0X88, 0X77}; //进入POWER DOWN 状濁命仿

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)//外部中断回调处理函数
{
	if(GPIO_Pin==GPIO_PIN_7)//指纹模块触发
	{
	
		
		if(press_FR())
		{
			step=2;
			YS_SYN_Set(SYN_SuspendCom);
			HAL_Delay(1000);
				HAL_UART_Transmit(&huart1,(uint8_t *)"指纹验证成功",16,100);
			SYN_FrameInfo(0, (uint8_t *)"[v7][m1][t5]指纹验证成功");
			
		}
		else
		{
				YS_SYN_Set(SYN_SuspendCom);
			HAL_Delay(1000);
			SYN_FrameInfo(0, (uint8_t *)"[v7][m1][t5]请验证指纹");
			HAL_UART_Transmit(&huart1,(uint8_t *)"指纹验证失败",16,100);
		}
		
		
		
		
	}
	if(GPIO_Pin==GPIO_PIN_6)//气流传感器触发
	{
		uint8_t hp=100;//超时退出
		uint8_t num=20;
		uint16_t max_adcValue=0;
		
		HAL_ADC_Start_IT(&hadc1);
		while(!hadc1.Instance->SR&&hp--);//等待adc转换完成
		
		while(hp)
		{
			HAL_Delay(10);
			HAL_ADC_Start_IT(&hadc1);
			
			uint32_t adcValue = HAL_ADC_GetValue(&hadc1);
			if(max_adcValue<adcValue)//获取最大值
			{
				max_adcValue=adcValue;
				
			}
			num--;
			if(num<=0)
			{
				
				break;
				
			}
			
		}
		HAL_ADC_Stop_IT(&hadc1);
		float f1=max_adcValue /4095.0;
		float f2=(3.3 / 5.0)*1000.0;
		
		
		
		float alcoholPpm = f1*f2;//将电压值转化成ppm为单位的值
		uint8_t bac=(uint8_t)(0.21*alcoholPpm);//
		if(bac<=30)//矫正基础值
		{
			bac=0;
			
		}
		
		char str[32]="";
		uint8_t len=sprintf(str,"血液中酒精浓度为 : %d mg/100ml ",bac);		
		HAL_UART_Transmit(&huart1,(uint8_t *)str,len,100);
		
		
	}
	
	
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//定时器中断回调函数
{
	if(htim==&htim2 &&step==1)
	{
		playSoundFlag=1;
		
		
	
	
		
		
		
	
		
	}
	
	
	
	
	
	
	
}






