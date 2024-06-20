#include "MyCallbackFunc.h"
uint8_t num=3;
uint8_t step=1;
volatile uint8_t playSoundFlag = 0;//声音播放开关
volatile uint8_t Result_fgPrt=0;//指纹测试结果 0 失败 1 通过
 uint8_t Resule_AlcoholTest=0;//酒精测试结果 0 失败  1 通过
volatile uint8_t Alcohol_threshold=60;//酒精测试阈值，大于这个值会触发提醒


uint8_t SYN_StopCom[] = {0xFD, 0X00, 0X02, 0X02, 0XFD}; //停止合成
uint8_t SYN_SuspendCom[] = {0XFD, 0X00, 0X02, 0X03, 0XFC}; //暂停合成
uint8_t SYN_RecoverCom[] = {0XFD, 0X00, 0X02, 0X04, 0XFB}; //恢复合成
uint8_t SYN_ChackCom[] = {0XFD, 0X00, 0X02, 0X21, 0XDE}; //状濁查诿
uint8_t SYN_PowerDownCom[] = {0XFD, 0X00, 0X02, 0X88, 0X77}; //进入POWER DOWN 状濁命仿

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)//外部中断回调处理函数
{
	if(GPIO_Pin==GPIO_PIN_7)//指纹模块触发
	{
	     __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);
        HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
        
		
		if(press_FR())//指纹验证成功
		{
		num=3;
		step=2;	
		Result_fgPrt=1;
		
		htim2.Instance->ARR=12000;//6秒触发一次
		HAL_TIM_Base_Start_IT(&htim2);
		HAL_UART_Transmit(&huart1,(uint8_t *)"指纹验证成功",20,100);
			
			
		}
		else
		{
			step=2;
			Result_fgPrt=0;
			HAL_UART_Transmit(&huart1,(uint8_t *)"指纹验证失败",20,100);
			playSoundFlag=1;
		}
		
		
		HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
		
	}
	if(GPIO_Pin==GPIO_PIN_6)//气流传感器触发
	{
		if(step==2&&Result_fgPrt==1)
		{
			
			
		
		step=3;
		uint8_t hp=100;//超时退出
		uint8_t num_value=20;
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
			num_value--;
			if(num_value<=0)
			{
				
				break;
				
			}
			
		}
		HAL_ADC_Stop_IT(&hadc1);
		float f1=max_adcValue /4095.0;
		float f2=(3.3 / 5.0)*1000.0;
		
		
		
		float alcoholPpm = f1*f2;//将电压值转化成ppm为单位的值
		uint8_t bac=(uint8_t)(0.21*alcoholPpm);//
		
		
		
			HAL_TIM_Base_Stop_IT(&htim2);//先关闭定时器
			htim2.Instance->ARR=12000;//设置间隔时间
			HAL_TIM_Base_Start_IT(&htim2);
		if(bac<=Alcohol_threshold)
		{
			num=3;
			Resule_AlcoholTest=1;//通过酒精测试
		}else
		{
			Resule_AlcoholTest=0;
			
		}
		if(bac<=30)//矫正基础值
		{
			bac=0;
			
		}
		
		char str[32]="";
		uint8_t len=sprintf(str,"血液中酒精浓度为 : %d mg/100ml ",bac);		
		HAL_UART_Transmit(&huart1,(uint8_t *)str,len,100);
		
		
	}
}
	
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//定时器中断回调函数
{
	if(htim==&htim2 &&step==1)
	{
		num--;
		playSoundFlag=1;
		
		if(num<=0)
		{
			
			HAL_TIM_Base_Stop_IT(&htim2);
			
			
		}
	}
	if(htim==&htim2 &&step==2)
	{
		num--;
		playSoundFlag=1;
			if(num<=0)
		{
			
			HAL_TIM_Base_Stop_IT(&htim2);
			
			
		}
		
	}
	
		
		if(htim==&htim2 &&step==3)
		{
			
			__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);
			__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_7);
      HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);//关闭外部中断
			playSoundFlag=1;
			if(Resule_AlcoholTest==1)
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
		
	
		

	
	
	
	
	
	
	




}


