#include "MyCallbackFunc.h"
int num=3;
int step=0;
extern uint8_t Drink_switches;
volatile uint8_t playSoundFlag = 0;//声音播放开关
volatile uint8_t Result_fgPrt=0;//指纹测试结果 0 失败 1 通过
int Resule_AlcoholTest=0;//酒精测试结果 0 失败  1 通过
volatile uint8_t Start_AlcoholTest=0;
int time=8;//
int shi=0;
int fen=0;
int miao=0;
uint8_t fp_start=0;
uint8_t PILAO=0;
uint16_t LED_PIN=0XFFFF;
GPIO_TypeDef * LED_PORT=NULL;




uint8_t SYN_StopCom[] = {0xFD, 0X00, 0X02, 0X02, 0XFD}; //停止合成
uint8_t SYN_SuspendCom[] = {0XFD, 0X00, 0X02, 0X03, 0XFC}; //暂停合成
uint8_t SYN_RecoverCom[] = {0XFD, 0X00, 0X02, 0X04, 0XFB}; //恢复合成
uint8_t SYN_ChackCom[] = {0XFD, 0X00, 0X02, 0X21, 0XDE}; //状濁查诿
uint8_t SYN_PowerDownCom[] = {0XFD, 0X00, 0X02, 0X88, 0X77}; //进入POWER DOWN 状濁命仿
 
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)//外部中断回调处理函数
{
	if(GPIO_Pin==GPIO_PIN_7&&step==1)//指纹模块触发
	{
	     fp_start=1;
		 
		
	
		
		
	
		
	}
	if(GPIO_Pin==GPIO_PIN_6&&step==2)//气流传感器触发
	{
		 __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);
			Start_AlcoholTest=1;
		HAL_UART_Transmit(&huart1,"触发了",6,100);
	
		

}

	
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//定时器中断回调函数
{
	
  HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);//关闭外部中断
		if(htim==&htim2 &&step==0)
	{
		
			playSoundFlag=1;
		
	}
	
	
	if(htim==&htim2 &&step==0)
	{
		
			playSoundFlag=1;
		
	}
	else if(htim==&htim2 &&step==1)
	{
	
		
		
		if(num<=0)
		{
			
			HAL_TIM_Base_Stop_IT(&htim2);

		}
		else
		{
		playSoundFlag=1;
				num--;
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
		if(htim==&htim4)//LED闪烁控制
		{
			if(LED_PORT!=NULL)
			{
			
			HAL_GPIO_TogglePin(LED_PORT,LED_PIN);
			
			}
			
			
		}
			if(htim==&htim6)//疲劳时间定时 1秒触发一次
		{
			
			
			miao++;
			if(miao==60)
			{
				fen++;
				miao=0;
				
			}
			if(fen==60)
			{
				shi++;
				fen=0;
				if(shi==time)
			{
				
				PILAO=1;
				
			}
				
				
			}
			
			
//			char buf[64]="";
//			sprintf(buf,"%d %d",miao,fen);		
//			HAL_UART_Transmit(&huart1,buf,6,100);
			
			
		}
	
		

	
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	
	
	
	
	
	




}


void Car_Status(uint8_t status)
{
	
	if(status)
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);		
	}
	
	
}








void LED_FastFlush(GPIO_TypeDef * PORT_TYPE,uint16_t PORT_PIN)
{
	LED_PORT=PORT_TYPE;
	LED_PIN=PORT_PIN;
	HAL_TIM_Base_Stop_IT(&htim4);
	htim4.Instance->ARR=3000;
	HAL_TIM_Base_Start_IT(&htim4);
}
void LED_SlowFlush(GPIO_TypeDef * PORT_TYPE,uint16_t PORT_PIN)
{
	
	LED_PORT=PORT_TYPE;
	LED_PIN=PORT_PIN;
	HAL_TIM_Base_Stop_IT(&htim4);
	htim4.Instance->ARR=6000;
	HAL_TIM_Base_Start_IT(&htim4);
	
}
void LED_Constantlight(GPIO_TypeDef * PORT_TYPE,uint16_t PORT_PIN)
{
	LED_PORT=PORT_TYPE;
	LED_PIN=PORT_PIN;
	HAL_TIM_Base_Stop_IT(&htim4);
	HAL_GPIO_WritePin(LED_PORT,PORT_PIN,GPIO_PIN_RESET);
	
}

void LED_Off(GPIO_TypeDef * PORT_TYPE,uint16_t PORT_PIN)
{
	LED_PORT=PORT_TYPE;
	LED_PIN=PORT_PIN;
	HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_SET);
	
	
}



//#define LED_FP              0
//#define LED_Alcohol         1
//#define LED_Error           2
//#define LED_Ready           3
//#define LED_JiuJinChaoBiao  4
//#define LED_PiNaoJiaShi     5
//#define LED_OverRange       6
//#define LED_Storage         7
void LED_Control(uint8_t LedType,uint8_t ways)
{
	switch(LedType)
	{
		case  0 : 
		if(ways==0)     {LED_Off(GPIOD,GPIO_PIN_6);}
		else if(ways==1){ LED_Constantlight(GPIOD,GPIO_PIN_6);}
		else if(ways==2){ LED_SlowFlush(GPIOD,GPIO_PIN_6);}
		else            { LED_FastFlush(GPIOD,GPIO_PIN_6);}
		break;
		
		case  1 : 
		if(ways==0)     {LED_Off(GPIOG,GPIO_PIN_15);}
		else if(ways==1){ LED_Constantlight(GPIOG,GPIO_PIN_15);}
		else if(ways==2){ LED_SlowFlush(GPIOG,GPIO_PIN_15);}
		else            { LED_FastFlush(GPIOG,GPIO_PIN_15);}
		break;
		
		case  2 : 
		if(ways==0)     {LED_Off(GPIOC,GPIO_PIN_8);}
		else if(ways==1){ LED_Constantlight(GPIOC,GPIO_PIN_8);}
		else if(ways==2){ LED_SlowFlush(GPIOC,GPIO_PIN_8);}
		else            { LED_FastFlush(GPIOC,GPIO_PIN_8);}
		break;
		
		case  3 : 
		if(ways==0)     {LED_Off(GPIOC,GPIO_PIN_0);}
		else if(ways==1){ LED_Constantlight(GPIOC,GPIO_PIN_0);}
		else if(ways==2){ LED_SlowFlush(GPIOC,GPIO_PIN_0);}
		else            { LED_FastFlush(GPIOC,GPIO_PIN_0);}
		break;
		
		case  4 : 
		if(ways==0)     {LED_Off(GPIOG,GPIO_PIN_6);}
		else if(ways==1){ LED_Constantlight(GPIOG,GPIO_PIN_6);}
		else if(ways==2){ LED_SlowFlush(GPIOG,GPIO_PIN_6);}
		else            { LED_FastFlush(GPIOG,GPIO_PIN_6);}
		break;
		
		case  5 : 
		if(ways==0)     {LED_Off(GPIOG,GPIO_PIN_7);}
		else if(ways==1){ LED_Constantlight(GPIOG,GPIO_PIN_7);}
		else if(ways==2){ LED_SlowFlush(GPIOG,GPIO_PIN_7);}
		else            { LED_FastFlush(GPIOG,GPIO_PIN_7);}
		break;
		
		case  6 : 
		if(ways==0)     {LED_Off(GPIOG,GPIO_PIN_8);}
		else if(ways==1){ LED_Constantlight(GPIOG,GPIO_PIN_8);}
		else if(ways==2){ LED_SlowFlush(GPIOG,GPIO_PIN_8);}
		else            { LED_FastFlush(GPIOG,GPIO_PIN_8);}
		break;
		
		case  7 : 
		if(ways==0)     {LED_Off(GPIOA,GPIO_PIN_11);}
		else if(ways==1){ LED_Constantlight(GPIOA,GPIO_PIN_11);}
		else if(ways==2){ LED_SlowFlush(GPIOA,GPIO_PIN_11);}
		else            { LED_FastFlush(GPIOA,GPIO_PIN_11);}
		break;

		
		
		
		
		
	}
	
	
	
}



void remove_newline(char *str)
{
	
	    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        if (*src != '\n' && *src != '\r') {
            *dst++ = *src;
        }
    }
    *dst = '\0';
	
	
	
}

