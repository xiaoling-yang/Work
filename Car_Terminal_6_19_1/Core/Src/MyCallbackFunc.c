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
		uint8_t max_adcValue=0;
		HAL_UART_Transmit(&huart1,(uint8_t *)"吹气了",10,100);
		HAL_ADC_Start(&hadc1);
		while(!hadc1.Instance->SR&&hp--);//等待adc转换完成
		
		while(hadc1.Instance->SR)
		{
			HAL_ADC_Start(&hadc1);
			
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
		HAL_ADC_Stop(&hadc1);
		
		float alcoholPpm = max_adcValue * (1000.0 / 4095.0);//将电压值转化成ppm为单位的值
		float bac=0.00021*alcoholPpm;//
		
		char str[32]="";
		sprintf(str,"%f",bac);		
		HAL_UART_Transmit(&huart1,(uint8_t *)str,10,100);
		
		
	}
	
	
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//定时器中断回调函数
{
	if(htim==&htim2)
	{
		HAL_UART_Transmit(&huart1,(uint8_t *)"nihao",4,100);
		
	}
	
	
	
	
	
	
	
}






