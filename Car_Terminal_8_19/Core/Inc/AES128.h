/*************************************************************************************************************
 * 文件名		:	AES128.h
 * 功能			:	AES128加密/解密操作
 * 作者			:	cp1300@139.com
 * 创建时间		:	2022-02-15
 * 最后修改时间	:	2022-02-15
 * 详细			:	基于CSDN网友分享的代码修改，去掉了内存申请，修改最小单位为16字节（可以不满16字节）的加密与解密，增加句柄方式，支持重入,修改为无符号8位类型数据加密解密
					加密方式为：AES128 ECB模式，填充zeropadding 数据块128位
					加密与解密数据缓冲区必须是16字节整数倍,实际数据长度不够会自动补充0,但是缓冲区大小一定是16字节整数倍
*************************************************************************************************************/
#ifndef _AES128_H_
#define _AES128_H_
#include "main.h"
 
 
//AES加密模块句柄(注意会很占用内存，小心堆栈不足)
typedef struct  
{
	//测试发现最大只需要176字节，44*4
	uint8_t RoundKey[176];	// The array that stores the round keys.
	// in - it is the array that holds the CipherText to be decrypted.
	// out - it is the array that holds the output of the for decryption.
	// state - the array that holds the intermediate results during decryption.
	unsigned char state[4][4];
	// The Key input to the AES Program
	unsigned char Key[32];
	uint8_t* pDataBuff;				//当前要解析的16字节数据缓冲区指针,输入输出共用
}AES128_HANDLE;
 
 
void AES128_SetKey(AES128_HANDLE* pHandle, const uint8_t Key[16]);					//AES128设置KEY-必须先调用初始化KEY
void AES128_Encrypt128bit(AES128_HANDLE* pHandle, uint8_t pData[16], uint8_t DataSize);	//子操作:加密128bit数据
uint32_t  AES128_EncryptData(AES128_HANDLE* pHandle, uint8_t* pData, uint32_t DataSize);		//AES128加密数据
void AES128_Decrypt128bit(AES128_HANDLE* pHandle, uint8_t pData[16]);				//子操作:AES128解密128bit数据
void AES128_DecryptData(AES128_HANDLE* pHandle, uint8_t* pData, uint32_t DataSize);		//AES128解密数据
 
#endif //_AES128_H_
