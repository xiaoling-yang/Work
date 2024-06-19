#include "MyCallbackFunc.h"
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)//外部中断回调处理函数
{
	if(GPIO_Pin==GPIO_PIN_7)
	{
	
		 Add_FR();	
		//press_FR();
		HAL_UART_Transmit(&huart1,(uint8_t *)"touch",5,100);
		
		
	}
	if(GPIO_Pin==GPIO_PIN_6)
	{
		uint8_t hp=100;//超时退出
		uint8_t num=20;
		uint16_t max_adcValue=0;
		
		HAL_ADC_Start_IT(&hadc1);
		while(!hadc1.Instance->SR&&hp--);//等待adc转换完成
		
		while(1)
		{
			HAL_Delay(10);
			HAL_ADC_Start_IT(&hadc1);
			
			uint32_t adcValue = HAL_ADC_GetValue(&hadc1);
			if(max_adcValue<adcValue)//
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
		uint8_t bac=0.21*alcoholPpm;//
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
	if(htim==&htim2)
	{
		HAL_UART_Transmit(&huart1,(uint8_t *)"nihao",4,100);
		
	}
	
	
	
	
	
	
	
}






