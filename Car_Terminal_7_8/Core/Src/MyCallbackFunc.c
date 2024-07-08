#include "MyCallbackFunc.h"
int num=3;
int step=0;
extern uint8_t Drink_switches;
volatile uint8_t playSoundFlag = 0;//声音播放开关
volatile uint8_t Result_fgPrt=0;//指纹测试结果 0 失败 1 通过
int Resule_AlcoholTest=0;//酒精测试结果 0 失败  1 通过
volatile uint8_t Start_AlcoholTest=0;



uint8_t SYN_StopCom[] = {0xFD, 0X00, 0X02, 0X02, 0XFD}; //停止合成
uint8_t SYN_SuspendCom[] = {0XFD, 0X00, 0X02, 0X03, 0XFC}; //暂停合成
uint8_t SYN_RecoverCom[] = {0XFD, 0X00, 0X02, 0X04, 0XFB}; //恢复合成
uint8_t SYN_ChackCom[] = {0XFD, 0X00, 0X02, 0X21, 0XDE}; //状濁查诿
uint8_t SYN_PowerDownCom[] = {0XFD, 0X00, 0X02, 0X88, 0X77}; //进入POWER DOWN 状濁命仿
 
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)//外部中断回调处理函数
{
	if(GPIO_Pin==GPIO_PIN_7&&step==1)//指纹模块触发
	{
	     
		 __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_7);
       HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
       HAL_TIM_Base_Stop_IT(&htim2);//防止被定时器打断
		
		if(press_FR())//指纹验证成功
		{
		step=2;	
		num=3;
		
		Result_fgPrt=1;
		
		htim2.Instance->ARR=11000;//6秒触发一次
		
		HAL_UART_Transmit(&huart1,(uint8_t *)"指纹验证成功",20,100);
			
			
			
		}
		else
		{
			
			step=2;	
			Result_fgPrt=0;
			num=0;
			HAL_TIM_Base_Stop_IT(&htim2);
			HAL_UART_Transmit(&huart1,(uint8_t *)"指纹验证失败",20,100);
			playSoundFlag=1;
			
		}
		
		
	
		
	}
	if(GPIO_Pin==GPIO_PIN_6&&step==2)//气流传感器触发
	{
		 __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);
			Start_AlcoholTest=1;
	
		

}
 HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	HAL_TIM_Base_Start_IT(&htim2);
	
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//定时器中断回调函数
{
	
  HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);//关闭外部中断
	
	if(htim==&htim2 &&step==0)
	{
		
			playSoundFlag=1;
		
	}
	
	
	else if(htim==&htim2 &&step==1)
	{
		num--;
		
		
		if(num<=0)
		{
			
			HAL_TIM_Base_Stop_IT(&htim2);
			
			
		}
		else
		{
		playSoundFlag=1;
		}
	}
	else if(htim==&htim2 &&step==2)
	{
		
		
	
			if(num<=0)
		{
			
			HAL_TIM_Base_Stop_IT(&htim2);
			
			
		}
		else
		{
			playSoundFlag=1;
		}
		num--;
		
	}
	
		
		else if(htim==&htim2 &&step==3)
		{
			
			
			playSoundFlag=1;
			if(Resule_AlcoholTest==1||(Resule_AlcoholTest==2&&Drink_switches==1))
			{
				
			
				
				num--;
				
				if(num<=0)
				{
					HAL_TIM_Base_Stop_IT(&htim2);
					
					
				}
				
			}
			else
			{}
			
			
			
			
		}
		
	
		

	
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	
	
	
	
	
	




}


void Car_Status(uint8_t status)
{
	
	if(status)
	{
		
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);
	}else
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);
		
		
	}
	
}


