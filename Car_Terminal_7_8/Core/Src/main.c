/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "string.h"
#include "stdlib.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MyCallbackFunc.h"
#include "string.h"
#include "AS608.h"
#include "AES128.h"
#include "syn6288.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t count=0;
extern int step;
extern int Resule_AlcoholTest;
extern volatile uint8_t Result_fgPrt;//指纹测试结果
extern volatile uint8_t playSoundFlag ;//音乐播放开关
extern uint8_t SYN_StopCom[] ; //停止合成
extern uint8_t SYN_SuspendCom[]; //暂停合成
extern uint8_t SYN_RecoverCom[]; //恢复合成
extern uint8_t SYN_ChackCom[] ; //状濁查诿
extern uint8_t SYN_PowerDownCom[] ; //进入POWER DOWN 状濁命仿
extern volatile uint8_t Start_AlcoholTest;//酒精测试阈值，大于这个值会触发提醒
extern int num;
uint8_t Control=0;//云端是否控制 0 无控制 1 控制
uint8_t SW=0;// 云端控制可以强制启动和锁定车辆  0 强制关闭 1强制打开 
uint8_t Alcohol_value_Max=100; 
uint8_t Alcohol_value_Min=60;
uint8_t Drink_switches=0;//默认饮酒驾驶不允许开车 0关 1开

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	//当所有测试均通过时，拉低PC0引脚输出低电平信号，引脚默认是高电平
	
	//数据加密解密示例
	
//AES128_HANDLE AES128_Handle;
//	uint8_t DataBuff[64];//霿要加密的数据  
//        uint8_t key[16];//钥匙key
//		uint32_t len;//加密的长庿
//				memset(key, 0, 16);//将key清零
//	key[0] = '1';//设置钥匙丿1其余全零
//    AES128_SetKey(&AES128_Handle, key);//AES128设置KEY，?过key获得对应的句柿
//	len = AES128_EncryptData(&AES128_Handle, DataBuff, 20);//通过句柄去去对数据进行加寿
//	AES128_DecryptData(&AES128_Handle, DataBuff, len);	//子操使:AES128解密128bit数据，len表示要解密数据的长度
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_UART5_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
	//htim2.Instance->ARR=6000;
	
	
	
	//选择背景音乐2?(0：无背景音乐  1-15：背景音乐可逿)
		//m[0~16]:0背景音乐为静音，16背景音乐音量朿夿
		//v[0~16]:0朗读音量为静音，16朗读音量朿夿
		//t[0~5]:0朗读语?最慢，5朗读语?最忿
		//其他不常用功能请参迃数据手冿
		//SYN_FrameInfo(2, "[v7][m1][t5]欢迎使用绿深旗舰店SYN6288语音合成模块");
		//暂停合成，此时没有用到，用于展示函数用法
		//YS_SYN_Set(SYN_SuspendCom);

			
			if(Control==1)
			{
			
			 __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_7);
       HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
       HAL_TIM_Base_Stop_IT(&htim2);
			if(SW==0)//锁定车辆
			{
				
				HAL_TIM_Base_Start_IT(&htim2);
				Car_Status(0);
			}
			else//解锁车辆
			{
				SYN_FrameInfo(0, (uint8_t *)"[v16][m1][t5]车辆已解锁");
				Car_Status(1);        				
			}
			}
		else//进行正常指纹验证
		{
			
			num=3;
		}
		

  /* USER CODE END 2 */
		

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		
		
				
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		int distance=999;
		uint8_t n=0;
		char buf_recv[64]="";
		char data[64]="";
		char *str=NULL;
		char *str1=NULL;
		
		if(__HAL_UART_GET_FLAG(&huart6, UART_FLAG_RXNE))
		{
		HAL_UART_Receive(&huart6,buf_recv,sizeof(buf_recv),10);
		
		str=strtok(buf_recv,":");
		while(str!=NULL)
		{
			n++;
			str=strtok(NULL,":");
			
			if(n==2)
			{
				for(str1=strtok(str," ");str1!=NULL;str1=strtok(NULL,"m"))
				{
					HAL_UART_Transmit(&huart1,str1,strlen(str1),100);
					distance=atoi(str1);
					
				}
				
				
			}
		}
	}
		if(Control==0&&SW==0&&step==0)
		{
			
			if(distance<=300)
			{
				
				HAL_TIM_Base_Start_IT(&htim2);
				
				step=1;
			}
			else
			{
				if(playSoundFlag==1)
				{
					
				SYN_FrameInfo(0, (uint8_t *)"[v13][m1][t5]请靠近设备");
				HAL_UART_Transmit(&huart6,"111",3,100);
					playSoundFlag=0;
				}
			}
		}
//		
//		HAL_UART_Transmit(&huart1,data,10,10);
		
		if(step==0&&playSoundFlag==1)
		{
			if(Control==1&&SW==0)
			{
				
				
				playSoundFlag=0;
				SYN_FrameInfo(0, (uint8_t *)"[v11][m1][t5]车辆已锁定");	
				
				
			}
			
			
		}
		
		if( step==1 && playSoundFlag==1)//当前为第一步，且打开了语音
		{
			if(num)
			{
			playSoundFlag=0;//置零等待触发
			SYN_FrameInfo(0, (uint8_t *)"[v13][m1][t5]请验证指纹");	
			}
		}
		else if(step==2)
		{
			
			if(playSoundFlag&&Result_fgPrt==1 )
			{
				playSoundFlag=0;
				SYN_FrameInfo(0, (uint8_t *)"[v7][m1][t5]指纹验证成功，请进行酒精验证");
				
			}
			
			
		
		 __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_7);
			__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);
       HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
       HAL_TIM_Base_Stop_IT(&htim2);//防止被定时器打断
			if(Result_fgPrt)
			{
			
			
			
				
				
		if(Start_AlcoholTest==1)
		{
			
			
		
		step=3;
		uint8_t hp=10;//超时退出
		uint8_t num_value=20;
		uint16_t max_adcValue=0;
		
		
		
		while(hp--)
		{
			
			
			HAL_ADC_Start_IT(&hadc1);
			HAL_ADC_PollForConversion(&hadc1,100);//等待ADC转换完成
			uint32_t adcValue = HAL_ADC_GetValue(&hadc1);
			if(max_adcValue<adcValue)//获取最大值
			{
				max_adcValue=adcValue;
			}
			
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
		if(bac<Alcohol_value_Min)
		{
			num=3;
			Resule_AlcoholTest=1;//通过酒精测试
		}else if(bac>=Alcohol_value_Min&&bac<Alcohol_value_Max)//饮酒
		{
			
			num=3;
			Resule_AlcoholTest=2;
			
		}
		else
		{
			num=0;
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
			else
			{
			
				playSoundFlag=0;
				step=1;
				SYN_FrameInfo(0, (uint8_t *)"[v7][m1][t5]指纹验证失败");
			}
				HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
				HAL_TIM_Base_Start_IT(&htim2);	
		}
		
				else if( step==3 && playSoundFlag==1)//当前为第二步，且打开了语音
		{
					playSoundFlag=0;//置零等待触发
			if(Resule_AlcoholTest==1)//小于饮酒值
			{
			 SYN_FrameInfo(0, (uint8_t *)"[v7][m1][t5]酒精测试通过，请启动车辆");	
			}
			else if(Resule_AlcoholTest==2)//饮酒驾驶
			{
				if(Drink_switches==1)
				{
					SYN_FrameInfo(0, (uint8_t *)"[v7][m1][t5]饮酒驾驶，请启动车辆");	
				}
				else
				{
					SYN_FrameInfo(0, (uint8_t *)"[v7][m1][t5]饮酒驾驶，请关闭车辆");
				}
				
			}
			else
			{
				
				 SYN_FrameInfo(0, (uint8_t *)"[v7][m1][t5]酒精测试未通过，请关闭车辆");	
			}
		}
		
		

//		if(Resule_AlcoholTest)
//		{
//			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);//拉低PF0输出信号
//			
//		}
	
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
