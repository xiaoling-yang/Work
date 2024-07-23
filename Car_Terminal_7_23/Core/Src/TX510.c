#include "TX510.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"
#include "MyCallbackFunc.h"
extern volatile uint8_t uart_rx_complete ;
unsigned char UART5_RX_BUF[UART5_MAX_RECV_LEN];
unsigned char UART5_TX_BUF[UART5_MAX_SEND_LEN];
tx_result RT;
__IO uint16_t UART5_RX_STA=0;




static void MYUSART_sendData(uint8_t data)
{
	while((UART5->SR & 0X40)==0);
	UART5->DR=data;
	
	
}
static void SendHead(void)//����ͷ
{
	MYUSART_sendData(0xEF);
	MYUSART_sendData(0xAA);
	
	
}
static void SendCmd(uint8_t cmd)
{
	MYUSART_sendData(cmd);
}
static void SendMsgID(uint8_t MsgID)
{
	MYUSART_sendData(MsgID);
	
}
static void SendSize(uint8_t size1,uint8_t size2,uint8_t size3,uint8_t size4)
{
	MYUSART_sendData(size1);
	MYUSART_sendData(size2);
	MYUSART_sendData(size3);
	MYUSART_sendData(size4);	
}
static void SendParityCheck(uint8_t check)
{
	MYUSART_sendData(check);
}
static uint8_t *Judgestr(uint16_t waitTime)
{
	uint16_t temp=2000;
	uint8_t *data;
	uint8_t SUM=0;
	char *id_str;
	unsigned char head[2]={0XEF,0XAA};
	memset(&RT,0,sizeof(RT));
	memset(UART5_RX_BUF,0,UART5_MAX_RECV_LEN);

	
	while((UART5->SR & 0X40) == 0);

	HAL_UART_Receive(&huart5,(uint8_t *)UART5_RX_BUF,UART5_MAX_RECV_LEN,waitTime/4);//�������

	
	

     

	
	

	
	

	if(UART5_RX_BUF[0]==head[0] &&  UART5_RX_BUF[1]==head[1])
	{
		if(UART5_RX_BUF[8]==0X00)
		{
			
			RT.SyncWord=UART5_RX_BUF[0];
			RT.SyncWord<<=8;
			RT.SyncWord|=UART5_RX_BUF[1];
			RT.Reply_MsgID=UART5_RX_BUF[2];
			RT.MsgID=UART5_RX_BUF[7];
			RT.Result=UART5_RX_BUF[8];
			RT.User_id=UART5_RX_BUF[9];
			RT.User_id<<=8;
			RT.User_id |= UART5_RX_BUF[10];
			
			return 0;
		}
		
		
		
		
	}
	data=UART5_RX_BUF;
	

	 return data; 
	
}


//��������ע��
//�ɹ�����0 ID ��Ӧע���ID
//ʧ�ܷ���1  ID=NULL
uint8_t TX510_Register(uint16_t *ID)
{
	uint8_t temp=0;
	SendHead();
	SendMsgID(0x13);
	SendSize(0X00,0X00,0X00,0X00);
	temp=0x13+0x00+0x00+0x00+0x00;
	SendParityCheck(temp);
	if(Judgestr(2000)==0x00)
	{
		*ID=RT.User_id;
		return 0;
		
	}
	else
	{
		ID=NULL;
		
		return 1 ;
	}
	
}

//��������ʶ��
//����0ʶ��ɹ� 1ʧ��
uint8_t TX510_Identify(void)
{
	uint16_t *ID;
	uint8_t temp=0;
	SendHead();
	SendMsgID(0x12);
	SendSize(0x00,0x00,0x00,0x00);
	temp=0x12+0x00+0x00+0x00+0x00;
	SendParityCheck(0x12);
	

		if(Judgestr(2000)==0)
	{
		*ID=RT.User_id;
		
		HAL_UART_Transmit(&huart1,(uint8_t *)&RT.User_id,2,100);
		return 0;
		
	}
	else
	{
		
		
		return 1 ;
	}
	
	
}
//ɾ����ӦID���û�
//����0�ɹ�1ʧ��
//userIDΪ��Ҫɾ����ID

uint8_t TX510_DeletUser(uint16_t userID)
{
	

	uint8_t temp=0;
	SendHead();
	SendMsgID(0x20);
	SendSize(0x00,0x00,0x00,0x02);
	temp=0x20+0x00+0x00+0x00+0x02+userID;
	SendParityCheck(temp);
		if(Judgestr(2000)==0)
	{
		
		return 0;
		
	}
	else
	{
		
		
		return 1 ;
	}
	
	
}
//ɾ��ȫ���û�
//����0�ɹ�1ʧ��
uint8_t TX510_DeletAlluser(void)
{
	uint8_t temp=0;
	SendHead();
	SendMsgID(0x21);
	SendSize(0x00,0x00,0x00,0x00);
	temp=0x21+0x00+0x00+0x00+0x00;
	SendParityCheck(temp);
		if(Judgestr(2000)==0)
	{
		
		return 0;
		
	}
	else
	{
		
		
		return 1 ;
	}
	
	
	
}
//�򿪲����
//����0�򿪳ɹ���1ʧ��
uint8_t TX510_OpenLight(void)
{
	
	uint8_t temp=0;
	SendHead();
	SendMsgID(0xC2);
	SendSize(0x00,0x00,0x00,0x01);
	MYUSART_sendData(0X01);
	temp=0xC2+0X01+0x00+0x00+0x00+0x01;
	SendParityCheck(temp);
		if(Judgestr(2000)==0)
	{
		
		return 0;
		
	}
	else
	{
		
		
		return 1 ;
	}
	
	
	
}

//�رղ����
//����0�رճɹ���1ʧ��
uint8_t TX510_OffLight(void)
{
	
	uint8_t temp=0;
	SendHead();
	SendMsgID(0xC2);
	SendSize(0x00,0x00,0x00,0x01);
	MYUSART_sendData(0X00);
	temp=0xC2+0X00+0x00+0x00+0x00+0x01;
	SendParityCheck(temp);
		if(Judgestr(2000)==0)
	{
		
		return 0;
		
	}
	else
	{
		
		
		return 1 ;
	}
	
}
//����Ļ��ʾ������
//�ɹ�����0��ʧ�ܷ���1
uint8_t TX510_OpenDisplay(void)
{
	
	uint8_t temp=0;
	SendHead();
	SendMsgID(0xC1);
	SendSize(0x00,0x00,0x00,0x01);
	MYUSART_sendData(0X01);
	temp=0xC1+0X00+0x00+0x00+0x01+0x01;
	SendParityCheck(temp);
		if(Judgestr(2000)==0)
	{
		
		return 0;
		
	}
	else
	{
		
		
		return 1 ;
	}
	
}
//�ر���Ļ��ʾ������
//�ɹ�����0��ʧ�ܷ���1
uint8_t TX510_CloseDisplay(void)
{
	
	uint8_t temp=0;
	SendHead();
	SendMsgID(0xC1);
	SendSize(0x00,0x00,0x00,0x01);
	MYUSART_sendData(0X00);
	temp=0xC1+0X00+0x00+0x00+0x01+0x00;
	SendParityCheck(temp);
		if(Judgestr(2000)==0)
	{
		
		return 0;
		
	}
	else
	{
		
		
		return 1 ;
	}
	
}
//��������ʶ��
//����0�����ɹ���1ʧ��
uint8_t TX510_Restart(void)
{
	
	uint8_t temp=0;
	SendHead();
	SendMsgID(0xC3);
	SendSize(0x00,0x00,0x00,0x00);
	
	temp=0xC3+0X00+0x00+0x00+0x00;
	SendParityCheck(temp);
		if(Judgestr(2000)==0)
	{
		
		return 0;
		
	}
	else
	{
		
		
		return 1 ;
	}
	
}

//��ȡ�Ѿ�ע����û�����
//numΪ��ȡ�����û�����
//����0�����ɹ���1ʧ��
uint8_t TX510_ReadRegistedNum(uint8_t *num)
{
	
	uint8_t temp=0;
	SendHead();
	SendMsgID(0xC4);
	SendSize(0x00,0x00,0x00,0x00);
	
	temp=0xC4+0X00+0x00+0x00+0x00;
	SendParityCheck(temp);
		if(Judgestr(2000)==0)
	{
		*num=RT.User_id;
		
		return 0;
		
	}
	else
	{
		
		return 1;
		
	
}

}