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
extern volatile uint8_t Result_fgPrt;//ָ�Ʋ��Խ��
extern volatile uint8_t playSoundFlag ;//���ֲ��ſ���
extern uint8_t SYN_StopCom[] ; //ֹͣ�ϳ�
extern uint8_t SYN_SuspendCom[]; //��ͣ�ϳ�
extern uint8_t SYN_RecoverCom[]; //�ָ��ϳ�
extern uint8_t SYN_ChackCom[] ; //״�����
extern uint8_t SYN_PowerDownCom[] ; //����POWER DOWN ״������
extern volatile uint8_t Start_AlcoholTest;//�ƾ�������ֵ���������ֵ�ᴥ������
extern int num;
uint8_t Control=0;//�ƶ��Ƿ���� 0 �޿��� 1 ����
uint8_t SW=0;// �ƶ˿��ƿ���ǿ����������������  0 ǿ�ƹر� 1ǿ�ƴ� 
uint8_t Alcohol_value_Max=100; 
uint8_t Alcohol_value_Min=60;
uint8_t Drink_switches=0;//Ĭ�����Ƽ�ʻ�������� 0�� 1��

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	//�����в��Ծ�ͨ��ʱ������PC0��������͵�ƽ�źţ�����Ĭ���Ǹߵ�ƽ
	
	//���ݼ��ܽ���ʾ��
	
//AES128_HANDLE AES128_Handle;
//	uint8_t DataBuff[64];//�WҪ���ܵ�����  
//        uint8_t key[16];//Կ��key
//		uint32_t len;//���ܵĳ���
//				memset(key, 0, 16);//��key����
//	key[0] = '1';//����Կ��د1����ȫ��
//    AES128_SetKey(&AES128_Handle, key);//AES128����KEY��?��key��ö�Ӧ�ľ���
//	len = AES128_EncryptData(&AES128_Handle, DataBuff, 20);//ͨ�����ȥȥ�����ݽ��м���
//	AES128_DecryptData(&AES128_Handle, DataBuff, len);	//�Ӳ�ʹ:AES128����128bit���ݣ�len��ʾҪ�������ݵĳ���
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
	
	
	
	//ѡ�񱳾�����2?(0���ޱ�������  1-15���������ֿ��T)
		//m[0~16]:0��������Ϊ������16�������������c�B
		//v[0~16]:0�ʶ�����Ϊ������16�ʶ������c�B
		//t[0~5]:0�ʶ���?������5�ʶ���?���
		//���������ù�������}�����փ�
		//SYN_FrameInfo(2, "[v7][m1][t5]��ӭʹ�������콢��SYN6288�����ϳ�ģ��");
		//��ͣ�ϳɣ���ʱû���õ�������չʾ�����÷�
		//YS_SYN_Set(SYN_SuspendCom);

			
			if(Control==1)
			{
			
			 __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_7);
       HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
       HAL_TIM_Base_Stop_IT(&htim2);
			if(SW==0)//��������
			{
				
				HAL_TIM_Base_Start_IT(&htim2);
				Car_Status(0);
			}
			else//��������
			{
				SYN_FrameInfo(0, (uint8_t *)"[v16][m1][t5]�����ѽ���");
				Car_Status(1);        				
			}
			}
		else//��������ָ����֤
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
					
				SYN_FrameInfo(0, (uint8_t *)"[v13][m1][t5]�뿿���豸");
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
				SYN_FrameInfo(0, (uint8_t *)"[v11][m1][t5]����������");	
				
				
			}
			
			
		}
		
		if( step==1 && playSoundFlag==1)//��ǰΪ��һ�����Ҵ�������
		{
			if(num)
			{
			playSoundFlag=0;//����ȴ�����
			SYN_FrameInfo(0, (uint8_t *)"[v13][m1][t5]����ָ֤��");	
			}
		}
		else if(step==2)
		{
			
			if(playSoundFlag&&Result_fgPrt==1 )
			{
				playSoundFlag=0;
				SYN_FrameInfo(0, (uint8_t *)"[v7][m1][t5]ָ����֤�ɹ�������оƾ���֤");
				
			}
			
			
		
		 __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_7);
			__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);
       HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
       HAL_TIM_Base_Stop_IT(&htim2);//��ֹ����ʱ�����
			if(Result_fgPrt)
			{
			
			
			
				
				
		if(Start_AlcoholTest==1)
		{
			
			
		
		step=3;
		uint8_t hp=10;//��ʱ�˳�
		uint8_t num_value=20;
		uint16_t max_adcValue=0;
		
		
		
		while(hp--)
		{
			
			
			HAL_ADC_Start_IT(&hadc1);
			HAL_ADC_PollForConversion(&hadc1,100);//�ȴ�ADCת�����
			uint32_t adcValue = HAL_ADC_GetValue(&hadc1);
			if(max_adcValue<adcValue)//��ȡ���ֵ
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
		float alcoholPpm = f1*f2;//����ѹֵת����ppmΪ��λ��ֵ
		uint8_t bac=(uint8_t)(0.21*alcoholPpm);//
		
		
		
			HAL_TIM_Base_Stop_IT(&htim2);//�ȹرն�ʱ��
			htim2.Instance->ARR=12000;//���ü��ʱ��
			HAL_TIM_Base_Start_IT(&htim2);
		if(bac<Alcohol_value_Min)
		{
			num=3;
			Resule_AlcoholTest=1;//ͨ���ƾ�����
		}else if(bac>=Alcohol_value_Min&&bac<Alcohol_value_Max)//����
		{
			
			num=3;
			Resule_AlcoholTest=2;
			
		}
		else
		{
			num=0;
			Resule_AlcoholTest=0;
			
		}
		if(bac<=30)//��������ֵ
		{
			bac=0;
		}	
		char str[32]="";
		uint8_t len=sprintf(str,"ѪҺ�оƾ�Ũ��Ϊ : %d mg/100ml ",bac);		
		HAL_UART_Transmit(&huart1,(uint8_t *)str,len,100);	
	}
	
			}
			else
			{
			
				playSoundFlag=0;
				step=1;
				SYN_FrameInfo(0, (uint8_t *)"[v7][m1][t5]ָ����֤ʧ��");
			}
				HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
				HAL_TIM_Base_Start_IT(&htim2);	
		}
		
				else if( step==3 && playSoundFlag==1)//��ǰΪ�ڶ������Ҵ�������
		{
					playSoundFlag=0;//����ȴ�����
			if(Resule_AlcoholTest==1)//С������ֵ
			{
			 SYN_FrameInfo(0, (uint8_t *)"[v7][m1][t5]�ƾ�����ͨ��������������");	
			}
			else if(Resule_AlcoholTest==2)//���Ƽ�ʻ
			{
				if(Drink_switches==1)
				{
					SYN_FrameInfo(0, (uint8_t *)"[v7][m1][t5]���Ƽ�ʻ������������");	
				}
				else
				{
					SYN_FrameInfo(0, (uint8_t *)"[v7][m1][t5]���Ƽ�ʻ����رճ���");
				}
				
			}
			else
			{
				
				 SYN_FrameInfo(0, (uint8_t *)"[v7][m1][t5]�ƾ�����δͨ������رճ���");	
			}
		}
		
		

//		if(Resule_AlcoholTest)
//		{
//			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);//����PF0����ź�
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
