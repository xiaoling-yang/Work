#include "DeviceCtrol.h"
void AS608_Ctrol(uint8_t state)//指纹模块
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};	

	if(state==1)
	{
	GPIO_InitStruct.Pull =  GPIO_PULLDOWN;
	
	}
	else
	{
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		
	}
	GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	
}
void MQ3_Ctrol(uint8_t state)//控制酒精传感器是否开始采集
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};	
	if(state==1)
	{
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	
	}
	else
	{
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		
	}
	
	GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  
  
	
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	
}
	

