#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "as608.h"

uint8_t flag_Fingerprint_library=0;//指纹库状态  0 未满  1 满了
uint32_t AS608Addr = 0XFFFFFFFF; //默认
char str2[6] = {0};

//串口接收缓存区 	
uint8_t USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.
uint8_t USART2_TX_BUF[USART2_MAX_SEND_LEN]; 			  //发送缓冲,最大USART2_MAX_SEND_LEN字节

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
__IO uint16_t USART2_RX_STA=0;



//串口发送一个字节
static void MYUSART_SendData(uint8_t data)
{
  while((USART2->SR & 0X40) == 0);
  USART2->DR = data;
}
//发送包头
static void SendHead(void)
{
  MYUSART_SendData(0xEF);
  MYUSART_SendData(0x01);
}
//发送地址
static void SendAddr(void)
{
  MYUSART_SendData(AS608Addr >> 24);
  MYUSART_SendData(AS608Addr >> 16);
  MYUSART_SendData(AS608Addr >> 8);
  MYUSART_SendData(AS608Addr);
}
//发送包标识,
static void SendFlag(uint8_t flag)
{
  MYUSART_SendData(flag);
}
//发送包长度
static void SendLength(int length)
{
  MYUSART_SendData(length >> 8);
  MYUSART_SendData(length);
}
//发送指令码
static void Sendcmd(uint8_t cmd)
{
  MYUSART_SendData(cmd);
}
//发送校验和
static void SendCheck(uint16_t check)
{
  MYUSART_SendData(check >> 8);
  MYUSART_SendData(check);
}
//判断中断接收的数组有没有应答包
//waittime为等待中断接收数据的时间（单位1ms）
//返回值：数据包首地址
static uint8_t *JudgeStr(uint16_t waittime)
{
  char *data;
  uint8_t str[8];
  str[0] = 0xEF;
  str[1] = 0x01;
  str[2] = AS608Addr >> 24;
  str[3] = AS608Addr >> 16;
  str[4] = AS608Addr >> 8;
  str[5] = AS608Addr;
  str[6] = 0x07;
  str[7] = '\0';
//  USART2_RX_STA = 0;
		//HAL库的写法
	while (!(USART2->SR & USART_SR_TC));//等待串口发送完成
	HAL_UART_Receive(&huart2,(uint8_t *)USART2_RX_BUF,USART2_MAX_RECV_LEN,waittime/4);//串口三接收数据
	if(!memcmp(str,USART2_RX_BUF,7))//比对数据
	{
		  data = strstr((const char*)USART2_RX_BUF, (const char*)str);
      if(data)
        return (uint8_t*)data;
	}
	//
//  while(--waittime)
//  {
//    HAL_Delay(1);
//    if(USART2_RX_STA & 0X8000) //接收到一次数据
//    {
//      USART2_RX_STA = 0;
//      data = strstr((const char*)USART2_RX_BUF, (const char*)str);
//      if(data)
//        return (uint8_t*)data;
//    }
//  }
  return 0;
}
//录入图像 PS_GetImage
//功能:探测手指，探测到后录入指纹图像存于ImageBuffer。
//模块返回确认字
uint8_t PS_GetImage(void)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
  SendLength(0x03);
  Sendcmd(0x01);
  temp =  0x01 + 0x03 + 0x01;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
//生成特征 PS_GenChar
//功能:将ImageBuffer中的原始图像生成指纹特征文件存于CharBuffer1或CharBuffer2
//参数:BufferID --> charBuffer1:0x01	charBuffer1:0x02
//模块返回确认字
uint8_t PS_GenChar(uint8_t BufferID)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
  SendLength(0x04);
  Sendcmd(0x02);
  MYUSART_SendData(BufferID);
  temp = 0x01 + 0x04 + 0x02 + BufferID;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
//精确比对两枚指纹特征 PS_Match
//功能:精确比对CharBuffer1 与CharBuffer2 中的特征文件
//模块返回确认字
uint8_t PS_Match(void)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
  SendLength(0x03);
  Sendcmd(0x03);
  temp = 0x01 + 0x03 + 0x03;
  SendCheck(0x07);
	
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
//搜索指纹 PS_Search
//功能:以CharBuffer1或CharBuffer2中的特征文件搜索整个或部分指纹库.若搜索到，则返回页码。
//参数:  BufferID @ref CharBuffer1	CharBuffer2
//说明:  模块返回确认字，页码（相配指纹模板）
uint8_t PS_Search(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, SearchResult *p)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
  SendLength(0x08);
  Sendcmd(0x04);
  MYUSART_SendData(BufferID);
  MYUSART_SendData(StartPage >> 8);
  MYUSART_SendData(StartPage);
  MYUSART_SendData(PageNum >> 8);
  MYUSART_SendData(PageNum);
  temp = 0x01 + 0x08 + 0x04 + BufferID
         + (StartPage >> 8) + (uint8_t)StartPage
         + (PageNum >> 8) + (uint8_t)PageNum;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
  {
    ensure = data[9];
    p->pageID   = (data[10] << 8) + data[11];
    p->mathscore = (data[12] << 8) + data[13];
  }
  else
    ensure = 0xff;
  return ensure;
}
//合并特征（生成模板）PS_RegModel
//功能:将CharBuffer1与CharBuffer2中的特征文件合并生成 模板,结果存于CharBuffer1与CharBuffer2
//说明:  模块返回确认字
uint8_t PS_RegModel(void)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
  SendLength(0x03);
  Sendcmd(0x05);
  temp = 0x01 + 0x03 + 0x05;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
//储存模板 PS_StoreChar
//功能:将 CharBuffer1 或 CharBuffer2 中的模板文件存到 PageID 号flash数据库位置。
//参数:  BufferID @ref charBuffer1:0x01	charBuffer1:0x02
//       PageID（指纹库位置号）
//说明:  模块返回确认字
uint8_t PS_StoreChar(uint8_t BufferID, uint16_t PageID)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
  SendLength(0x06);
  Sendcmd(0x06);
  MYUSART_SendData(BufferID);
  MYUSART_SendData(PageID >> 8);
  MYUSART_SendData(PageID);
  temp = 0x01 + 0x06 + 0x06 + BufferID
         + (PageID >> 8) + (uint8_t)PageID;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
//删除模板 PS_DeletChar
//功能:  删除flash数据库中指定ID号开始的N个指纹模板
//参数:  PageID(指纹库模板号)，N删除的模板个数。
//说明:  模块返回确认字
uint8_t PS_DeletChar(uint16_t PageID, uint16_t N)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
  SendLength(0x07);
  Sendcmd(0x0C);
  MYUSART_SendData(PageID >> 8);
  MYUSART_SendData(PageID);
  MYUSART_SendData(N >> 8);
  MYUSART_SendData(N);
  temp = 0x01 + 0x07 + 0x0C
         + (PageID >> 8) + (uint8_t)PageID
         + (N >> 8) + (uint8_t)N;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
//清空指纹库 PS_Empty
//功能:  删除flash数据库中所有指纹模板
//参数:  无
//说明:  模块返回确认字
uint8_t PS_Empty(void)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
  SendLength(0x03);
  Sendcmd(0x0D);
  temp = 0x01 + 0x03 + 0x0D;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
//写系统寄存器 PS_WriteReg
//功能:  写模块寄存器
//参数:  寄存器序号RegNum:4\5\6
//说明:  模块返回确认字
uint8_t PS_WriteReg(uint8_t RegNum, uint8_t DATA)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
  SendLength(0x05);
  Sendcmd(0x0E);
  MYUSART_SendData(RegNum);
  MYUSART_SendData(DATA);
  temp = RegNum + DATA + 0x01 + 0x05 + 0x0E;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  if(ensure == 0)
		HAL_UART_Transmit(&huart1,"设置参数成功！",16,100);
    
  else
    HAL_UART_Transmit(&huart1,"设置参数失败！",16,100);
  return ensure;
}
//读系统基本参数 PS_ReadSysPara
//功能:  读取模块的基本参数（波特率，包大小等)
//参数:  无
//说明:  模块返回确认字 + 基本参数（16bytes）
uint8_t PS_ReadSysPara(SysPara *p)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
  SendLength(0x03);
  Sendcmd(0x0F);
  temp = 0x01 + 0x03 + 0x0F;
  SendCheck(temp);
  data = JudgeStr(1000);
  if(data)
  {
    ensure = data[9];
    p->PS_max = (data[14] << 8) + data[15];
    p->PS_level = data[17];
    p->PS_addr = (data[18] << 24) + (data[19] << 16) + (data[20] << 8) + data[21];
    p->PS_size = data[23];
    p->PS_N = data[25];
  }
  else
    ensure = 0xff;
  if(ensure == 0x00)
  {
//		HAL_UART_Transmit(&huart1,"设置参数失败！",16,100);
//		HAL_UART_Transmit(&huart1,"设置参数失败！",16,100);
//		HAL_UART_Transmit(&huart1,"设置参数失败！",16,100);
//		HAL_UART_Transmit(&huart1,"设置参数失败！",16,100);
//    printf("\r\n模块最大指纹容量=%d", p->PS_max);
//    printf("\r\n对比等级=%d", p->PS_level);
//    printf("\r\n地址=%x", p->PS_addr);
//    printf("\r\n波特率=%d", p->PS_N * 9600);
  }
  else
    HAL_UART_Transmit(&huart1,"参数读取错误！",16,100);
  return ensure;
}
//设置模块地址 PS_SetAddr
//功能:  设置模块地址
//参数:  PS_addr
//说明:  模块返回确认字
uint8_t PS_SetAddr(uint32_t PS_addr)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
  SendLength(0x07);
  Sendcmd(0x15);
  MYUSART_SendData(PS_addr >> 24);
  MYUSART_SendData(PS_addr >> 16);
  MYUSART_SendData(PS_addr >> 8);
  MYUSART_SendData(PS_addr);
  temp = 0x01 + 0x07 + 0x15
         + (uint8_t)(PS_addr >> 24) + (uint8_t)(PS_addr >> 16)
         + (uint8_t)(PS_addr >> 8) + (uint8_t)PS_addr;
  SendCheck(temp);
  AS608Addr = PS_addr; //发送完指令，更换地址
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  AS608Addr = PS_addr;
  if(ensure == 0x00)
		HAL_UART_Transmit(&huart1,(uint8_t *)"设置地址成功！",16,100);
    
  else
		
   HAL_UART_Transmit(&huart1,(uint8_t *)"设置地址错误！",16,100);
  return ensure;
}
//功能： 模块内部为用户开辟了256bytes的FLASH空间用于存用户记事本,
//	该记事本逻辑上被分成 16 个页。
//参数:  NotePageNum(0~15),Byte32(要写入内容，32个字节)
//说明:  模块返回确认字
uint8_t PS_WriteNotepad(uint8_t NotePageNum, uint8_t *Byte32)
{
  uint16_t temp;
  uint8_t  ensure, i;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
  SendLength(36);
  Sendcmd(0x18);
  MYUSART_SendData(NotePageNum);
  for(i = 0; i < 32; i++)
  {
    MYUSART_SendData(Byte32[i]);
    temp += Byte32[i];
  }
  temp = 0x01 + 36 + 0x18 + NotePageNum + temp;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
//读记事PS_ReadNotepad
//功能：  读取FLASH用户区的128bytes数据
//参数:  NotePageNum(0~15)
//说明:  模块返回确认字+用户信息
uint8_t PS_ReadNotepad(uint8_t NotePageNum, uint8_t *Byte32)
{
  uint16_t temp;
  uint8_t  ensure, i;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
  SendLength(0x04);
  Sendcmd(0x19);
  MYUSART_SendData(NotePageNum);
  temp = 0x01 + 0x04 + 0x19 + NotePageNum;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
  {
    ensure = data[9];
    for(i = 0; i < 32; i++)
    {
      Byte32[i] = data[10 + i];
    }
  }
  else
    ensure = 0xff;
  return ensure;
}
//高速搜索PS_HighSpeedSearch
//功能：以 CharBuffer1或CharBuffer2中的特征文件高速搜索整个或部分指纹库。
//		  若搜索到，则返回页码,该指令对于的确存在于指纹库中 ，且登录时质量
//		  很好的指纹，会很快给出搜索结果。
//参数:  BufferID， StartPage(起始页)，PageNum（页数）
//说明:  模块返回确认字+页码（相配指纹模板）
uint8_t PS_HighSpeedSearch(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, SearchResult *p)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
  SendLength(0x08);
  Sendcmd(0x1b);
  MYUSART_SendData(BufferID);
  MYUSART_SendData(StartPage >> 8);
  MYUSART_SendData(StartPage);
  MYUSART_SendData(PageNum >> 8);
  MYUSART_SendData(PageNum);
  temp = 0x01 + 0x08 + 0x1b + BufferID
         + (StartPage >> 8) + (uint8_t)StartPage
         + (PageNum >> 8) + (uint8_t)PageNum;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
  {
    ensure = data[9];
    p->pageID 	= (data[10] << 8) + data[11];
    p->mathscore = (data[12] << 8) + data[13];
  }
  else
    ensure = 0xff;
  return ensure;
}
//读有效模板个数 PS_ValidTempleteNum
//功能：读有效模板个数
//参数: 无
//说明: 模块返回确认字+有效模板个数ValidN
uint8_t PS_ValidTempleteNum(uint16_t *ValidN)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
  SendLength(0x03);
  Sendcmd(0x1d);
  temp = 0x01 + 0x03 + 0x1d;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
  {
    ensure = data[9];
    *ValidN = (data[10] << 8) + data[11];
  }
  else
    ensure = 0xff;

  if(ensure == 0x00)
  {
		char str[64]="";
		sprintf(str,"有效指纹个数=%d",(data[10] << 8)+data[11]);
	 HAL_UART_Transmit(&huart1,(uint8_t *)str,20,100);
  //  printf("\r\n有效指纹个数=%d", (data[10] << 8) + data[11]);
  }
  else
   HAL_UART_Transmit(&huart1,(uint8_t *)"读取指纹数错误！",16,100);
  return ensure;
}
//与AS608握手 PS_HandShake,检查AS608是否正常
//参数: PS_Addr地址指针
//说明: 模块返新地址（正确地址）
uint8_t PS_HandShake(uint32_t *PS_Addr)
{
  SendHead();
  SendAddr();
  MYUSART_SendData(0X01);
  MYUSART_SendData(0X00);
  MYUSART_SendData(0X00);
  HAL_Delay(200);
  if(USART2_RX_STA & 0X8000) //接收到数据
  {
    if(//判断是不是模块返回的应答包
      USART2_RX_BUF[0] == 0XEF
      && USART2_RX_BUF[1] == 0X01
      && USART2_RX_BUF[6] == 0X07
    )
    {
      *PS_Addr = (USART2_RX_BUF[2] << 24) + (USART2_RX_BUF[3] << 16)
                 + (USART2_RX_BUF[4] << 8) + (USART2_RX_BUF[5]);
      USART2_RX_STA = 0;
      return 0;
    }
    USART2_RX_STA = 0;
  }
  return 1;
}
//模块应答包确认码信息解析
//功能：解析确认码错误信息返回信息
//参数: ensure
const char *EnsureMessage(uint8_t ensure)
{
  const char *p;
  switch(ensure)
  {
  case  0x00:
    p = "       OK       ";
    break;
  case  0x01:
    p = " 数据包接收错误 ";
    break;
  case  0x02:
    p = "传感器上没有手指";
    break;
  case  0x03:
    p = "录入指纹图像失败";
    break;
  case  0x04:
    p = " 指纹太干或太淡 ";
    break;
  case  0x05:
    p = " 指纹太湿或太糊 ";
    break;
  case  0x06:
    p = "  指纹图像太乱  ";
    break;
  case  0x07:
    p = " 指纹特征点太少 ";
    break;
  case  0x08:
    p = "  指纹不匹配    ";
    break;
  case  0x09:
    p = " 没有搜索到指纹 ";
    break;
  case  0x0a:
    p = "   特征合并失败 ";
    break;
  case  0x0b:
    p = "地址序号超出范围";
  case  0x10:
    p = "  删除模板失败  ";
    break;
  case  0x11:
    p = " 清空指纹库失败 ";
    break;
  case  0x15:
    p = "缓冲区内无有效图";
    break;
  case  0x18:
    p = " 读写FLASH出错  ";
    break;
  case  0x19:
    p = "   未定义错误   ";
    break;
  case  0x1a:
    p = "  无效寄存器号  ";
    break;
  case  0x1b:
    p = " 寄存器内容错误 ";
    break;
  case  0x1c:
    p = " 记事本页码错误 ";
    break;
  case  0x1f:
    p = "    指纹库满    ";
    break;
  case  0x20:
    p = "    地址错误    ";
    break;
  default :
    p = " 返回确认码有误 ";
    break;
  }
  return p;
}

//显示确认码错误信息
void ShowErrMessage(uint8_t ensure)
{
	HAL_UART_Transmit(&huart1,(uint8_t*)EnsureMessage(ensure),24,100);
  
}

void show_ID(uint8_t x,uint8_t y,uint8_t ID)
{
//		OLED_ShowStr(x,y,&ID,16);
//		OLED_RefreshRAM();
}


//录指纹
uint8_t Add_FR(void)
{
	uint16_t ValidN=0;
  uint8_t i, ensure, processnum = 0;
  uint16_t ID_NUM = 0x0000;
	
  while(1)
  {
    switch (processnum)
    {
    case 0:
      i++;
		HAL_UART_Transmit(&huart1,(uint8_t *)"    请按手指    ",20,100);
		
      ensure = PS_GetImage();
      if(ensure == 0x00)
      {
        ensure = PS_GenChar(CharBuffer1); //生成特征
        if(ensure == 0x00)
        {
					HAL_UART_Transmit(&huart1,(uint8_t *)"    指纹正常    ",20,100);
         
          i = 0;
          processnum = 1; //跳到第二步
        }
        else ShowErrMessage(ensure);
      }
      else HAL_UART_Transmit(&huart1,(uint8_t *)" 获取图像失败 ",20,100);
      break;

    case 1:
      i++;
		HAL_UART_Transmit(&huart1,(uint8_t *)"  请再按一次   ",20,100);
		HAL_Delay(500);
     
      ensure = PS_GetImage();
      if(ensure == 0x00)
      {
        ensure = PS_GenChar(CharBuffer2); //生成特征
        if(ensure == 0x00)
        {
					HAL_UART_Transmit(&huart1,(uint8_t *)"    指纹正常    ",20,100);
        
          i = 0;
          processnum = 2; //跳到第三步
        }
        else ShowErrMessage(ensure);
      }
      else ShowErrMessage(ensure);
      break;

    case 2:
			HAL_UART_Transmit(&huart1,(uint8_t *)"  对比两次指纹  ",20,100);
     // PS_Match();
      ensure = PS_Match();
      if(ensure == 0x00)
      {
				HAL_UART_Transmit(&huart1,(uint8_t *)"    对比成功    ",20,100);
        
        processnum = 3; //跳到第四步
      }
      else
      {
				HAL_UART_Transmit(&huart1,(uint8_t *)"    对比失败    ",20,100);
       
        ShowErrMessage(ensure);
        i = 0;
        processnum = 0; //跳回第一步
      }
      HAL_Delay(500);
      break;

    case 3:
			HAL_UART_Transmit(&huart1,(uint8_t *)"  生成指纹模板  ",24,100);
   
      //HAL_Delay(500);
      ensure = PS_RegModel();
      if(ensure == 0x00)
      {
				HAL_UART_Transmit(&huart1,(uint8_t *)"生成指纹模板成功",26,100);

        processnum = 4; //跳到第五步
				
      }
      else
      {
        processnum = 0;
        ShowErrMessage(ensure);
      }
      //HAL_Delay(1000);
      break;

    case 4:
			
		
		
			
     ensure = PS_ValidTempleteNum(&ValidN);
		if(ensure == 0x00)
		{
			if(ValidN==300)//指纹库满了
			{
				
				HAL_UART_Transmit(&huart1,(uint8_t *)"  指纹库已满  ",20,100);
				flag_Fingerprint_library=1;//指纹库已经满了
    
				ValidN=0;//重零开始覆盖
				break;
				
			}
			ID_NUM=ValidN;//ID是从0开始
		}
		else
		{
				HAL_UART_Transmit(&huart1,(uint8_t *)"  录入指纹失败  ",20,100);
        processnum = 0;
			HAL_UART_Transmit(&huart1,(uint8_t *)"有效指纹数查询失败  ",26,100);
        
			
		}
      ensure = PS_StoreChar(CharBuffer2,ID_NUM); //储存模板
      if(ensure == 0x00)
      {
				HAL_UART_Transmit(&huart1,(uint8_t *)"  录入指纹成功  ",20,100);
    
    
        return 0 ;
      }
      else
      {
        HAL_UART_Transmit(&huart1,(uint8_t *)"  录入指纹失败  ",20,100);
        processnum = 0;
        ShowErrMessage(ensure);
      }
      break;
    }
    HAL_Delay(400);
    if(i == 10) //超过5次没有按手指则退出
    {
      break;
    }
  }
	return 1;
}

SysPara AS608Para;//指纹模块AS608参数
//刷指纹
uint8_t press_FR(void)
{
	
  SearchResult seach;
  uint8_t ensure;
  char str[20];
	uint8_t flag=0;
	

  while(1)
  {
   
    ensure = PS_GetImage();
    if(ensure == 0x00) //获取图像成功
    {
			HAL_UART_Transmit(&huart1,(uint8_t *)"获取图像成功",18,100);
      ensure = PS_GenChar(CharBuffer1);
      if(ensure == 0x00) //生成特征成功
      {
				HAL_UART_Transmit(&huart1,(uint8_t *)"生成特征成功",18,100);
        ensure = PS_HighSpeedSearch(CharBuffer1, 0, 99, &seach);
        if(ensure == 0x00) //搜索成功
        {
					HAL_UART_Transmit(&huart1,(uint8_t *)"指纹验证成功",18,100);
				
          sprintf(str, " ID:%d 得分:%d ", seach.pageID, seach.mathscore);
         HAL_UART_Transmit(&huart1,(uint8_t *)str,16,100);
					return 1;
          
        }
        else
        {
					HAL_UART_Transmit(&huart1,(uint8_t *)"指纹验证失败",16,100);
         
          
        }
      }
      else
			{};
				HAL_UART_Transmit(&huart1,(uint8_t *)"请按手指",16,100);
    
    }
	
			
			return 0;
		
  }

}

//删除指纹
void Del_FR(void)
{
  uint8_t  ensure;
  uint16_t ID_NUM = 0;



  ensure = PS_DeletChar(ID_NUM, 1); //删除单个指纹
  if(ensure == 0)
  {
		HAL_UART_Transmit(&huart1,(uint8_t *)"  删除指纹成功 ",16,100);
  
  }
  else
   HAL_UART_Transmit(&huart1,(uint8_t *)"  删除指纹失败 ",16,100);
  HAL_Delay(1500);

}

void delete_ALL(void)
{
		uint8_t ensure;
   ensure = PS_Empty(); //清空指纹库
   if(ensure == 0)
   {
		 HAL_UART_Transmit(&huart1,(uint8_t *)" 清空指纹库成功",22,100);
   
   }
   else
	 {
		 HAL_UART_Transmit(&huart1,(uint8_t *)"清空指纹库失败 ",22,100);
     
   }
}



uint8_t AuraLedConfig(uint8_t state)
{
	
	
	
	uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
	uint8_t  speed;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
  SendLength(0x07);
  Sendcmd(0x35);
	if(state)
	{
  MYUSART_SendData(0x03);
	MYUSART_SendData(0x00);//速度
	MYUSART_SendData(0x07);//颜色
	MYUSART_SendData(0x00);//次数
	temp = 0x01 + 0x07 + 0x35+0x03+0x00+0x07+0x00;
	}
	else
	{
		
	MYUSART_SendData(0x04);
	MYUSART_SendData(0x00);//速度
	MYUSART_SendData(0x07);//颜色
	MYUSART_SendData(0x00);//次数
	temp = 0x01+0x07+0x35+0x04+0x00+0x07+0x00;
	}
	
	
	
	
  
       
  SendCheck(temp);
 
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
 
  
  return ensure;
	
}
