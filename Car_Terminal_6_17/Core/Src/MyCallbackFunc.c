#include "MyCallbackFunc.h"
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)//外部中断回调处理函数
{
	if(GPIO_Pin==GPIO_PIN_7)
	{
		HAL_UART_Transmit(&huart1,"touch",5,100);
		press_FR();
		
	}
	
	
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//定时器中断回调函数
{
	if(htim==&htim2)
	{
		HAL_UART_Transmit(&huart1,"nihao",4,100);
		
	}
	
	
	
	
	
	
	
}






