#ifndef __TX510_H
#define __TX510_H
#include "main.h"


#define USART2_MAX_RECV_LEN     1000
#define USART2_MAX_SEND_LEN     1000


typedef struct TX510_RESULT {
	uint16_t    SyncWord;
	uint8_t  Reply_MsgID;
	uint8_t        MsgID;
	uint8_t       Result;
	uint16_t     User_id;
	
} tx_result;



static void MYUSART_sendData(uint8_t data);
static void SendHead(void);//·¢ËÍÍ·
static void SendCmd(uint8_t cmd);
static void SendMsgID(uint8_t MsgID);
static void SendSize(uint8_t size1,uint8_t size2,uint8_t size3,uint8_t size4);
static void SendParityCheck(uint8_t check);
static uint8_t *Judgestr(uint16_t waitTime);

uint8_t TX510_Register(uint16_t *ID);
uint8_t TX510_Identify(void);
uint8_t TX510_DeletUser(uint16_t userID);
uint8_t TX510_DeletAlluser(void);
uint8_t TX510_OpenLight(void);
uint8_t TX510_OffLight(void);
uint8_t TX510_OpenDisplay(void);
uint8_t TX510_CloseDisplay(void);
uint8_t TX510_Restart(void);
uint8_t TX510_ReadRegistedNum(uint8_t *num);


#endif