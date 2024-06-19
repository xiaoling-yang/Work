/*************************************************************************************************************
 * 文件名		:	AES128.c
 * 功能			:	AES128加密/解密操作
 * 作者			:	cp1300@139.com
 * 创建时间		:	2022-02-15
 * 最后修改时间	:	2022-02-15
 * 详细			:	基于CSDN网友分享的代码修改，去掉了内存申请，修改最小单位为16字节（可以不满16字节）的加密与解密，增加句柄方式，支持重入,修改为无符号8位类型数据加密解密
					加密方式为：AES128 ECB模式，填充zeropadding 数据块128位
					加密与解密数据缓冲区必须是16字节整数倍,实际数据长度不够会自动补充0,但是缓冲区大小一定是16字节整数倍
*************************************************************************************************************/
#include "AES128.h"
#include "string.h"
 
//
 
 
 
// The number of columns comprising a state in AES. This is a constant in AES. Value=4
#define Nb 4
 
 
 
//cg_Nc: the length of Key(128, 192 or 256) only
static const int cg_Nc = 128;
// The number of 32 bit words in the key. It is simply initiated to zero. The actual value is recieved in the program.
static const int cg_Nk = 4;// cg_Nc / 32;
// The number of rounds in AES Cipher. It is simply initiated to zero. The actual value is recieved in the program.
static const int cg_Nr = 10;// cg_Nk + 6;
 
 
static const uint8_t cg_rsbox[256] =
{ 0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb
, 0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb
, 0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e
, 0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25
, 0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92
, 0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84
, 0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06
, 0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b
, 0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73
, 0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e
, 0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b
, 0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4
, 0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f
, 0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef
, 0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61
, 0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };
 
static uint8_t getSBoxInvert(uint8_t num)
{
	return cg_rsbox[num];
}
 
static const uint8_t cg_sbox[256] = {
	//0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };
static uint8_t getSBoxValue(uint8_t num)
{
	return cg_sbox[num];
}
 
// The round constant word array, cg_Rcon[i], contains the values given by 
// x to th e power (i-1) being powers of x (x is denoted as {02}) in the field GF(2^8)
// Note that i starts at 1, not 0).
static const uint8_t cg_Rcon[255] = {
	0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
	0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39,
	0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
	0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
	0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
	0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
	0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b,
	0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
	0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
	0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
	0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35,
	0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
	0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
	0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63,
	0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
	0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb };
 
// This function produces Nb(cg_Nr+1) round keys. The round keys are used in each round to decrypt the states. 
static void KeyExpansion(AES128_HANDLE *pHandle)
{
	int i, j;
	uint8_t temp[4], k;
 
	// The first round key is the key itself.
	for (i = 0; i < cg_Nk; i++)
	{
		pHandle->RoundKey[i * 4] = pHandle->Key[i * 4];
		pHandle->RoundKey[i * 4 + 1] = pHandle->Key[i * 4 + 1];
		pHandle->RoundKey[i * 4 + 2] = pHandle->Key[i * 4 + 2];
		pHandle->RoundKey[i * 4 + 3] = pHandle->Key[i * 4 + 3];
	}
 
	// All other round keys are found from the previous round keys.
	while (i < (Nb * (cg_Nr + 1)))
	{
		for (j = 0; j < 4; j++)
		{
			temp[j] = pHandle->RoundKey[(i - 1) * 4 + j];
		}
		if (i % cg_Nk == 0)
		{
			// This function rotates the 4 bytes in a word to the left once.
			// [a0,a1,a2,a3] becomes [a1,a2,a3,a0]
			{
				k = temp[0];
				temp[0] = temp[1];
				temp[1] = temp[2];
				temp[2] = temp[3];
				temp[3] = k;
			}
 
			// SubWord() is a function that takes a four-byte input word and 
			// applies the S-box to each of the four bytes to produce an output word.
			{
				temp[0] = getSBoxValue(temp[0]);
				temp[1] = getSBoxValue(temp[1]);
				temp[2] = getSBoxValue(temp[2]);
				temp[3] = getSBoxValue(temp[3]);
			}
 
			temp[0] = temp[0] ^ cg_Rcon[i / cg_Nk];
		}
		else if (cg_Nk > 6 && i % cg_Nk == 4)
		{
			{
				temp[0] = getSBoxValue(temp[0]);
				temp[1] = getSBoxValue(temp[1]);
				temp[2] = getSBoxValue(temp[2]);
				temp[3] = getSBoxValue(temp[3]);
			}
		}
		pHandle->RoundKey[i * 4 + 0] = pHandle->RoundKey[(i - cg_Nk) * 4 + 0] ^ temp[0];
		pHandle->RoundKey[i * 4 + 1] = pHandle->RoundKey[(i - cg_Nk) * 4 + 1] ^ temp[1];
		pHandle->RoundKey[i * 4 + 2] = pHandle->RoundKey[(i - cg_Nk) * 4 + 2] ^ temp[2];
		pHandle->RoundKey[i * 4 + 3] = pHandle->RoundKey[(i - cg_Nk) * 4 + 3] ^ temp[3];
		i++;
	}
}
 
// This function adds the round key to state.
// The round key is added to the state by an XOR function.
static void AddRoundKey(AES128_HANDLE* pHandle, int round)
{
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			pHandle->state[j][i] ^= pHandle->RoundKey[round * Nb * 4 + i * Nb + j];
		}
	}
}
 
// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
static void InvSubBytes(AES128_HANDLE* pHandle)
{
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			pHandle->state[i][j] = getSBoxInvert(pHandle->state[i][j]);
 
		}
	}
}
 
// The ShiftRows() function shifts the rows in the state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
static void InvShiftRows(AES128_HANDLE* pHandle)
{
	uint8_t temp;
 
	// Rotate first row 1 columns to right    
	temp = pHandle->state[1][3];
	pHandle->state[1][3] = pHandle->state[1][2];
	pHandle->state[1][2] = pHandle->state[1][1];
	pHandle->state[1][1] = pHandle->state[1][0];
	pHandle->state[1][0] = temp;
 
	// Rotate second row 2 columns to right    
	temp = pHandle->state[2][0];
	pHandle->state[2][0] = pHandle->state[2][2];
	pHandle->state[2][2] = temp;
 
	temp = pHandle->state[2][1];
	pHandle->state[2][1] = pHandle->state[2][3];
	pHandle->state[2][3] = temp;
 
	// Rotate third row 3 columns to right
	temp = pHandle->state[3][0];
	pHandle->state[3][0] = pHandle->state[3][1];
	pHandle->state[3][1] = pHandle->state[3][2];
	pHandle->state[3][2] = pHandle->state[3][3];
	pHandle->state[3][3] = temp;
}
 
// xtime is a macro that finds the product of {02} and the argument to xtime modulo {1b}  
#define xtime(x)   ((x<<1) ^ (((x>>7) & 1) * 0x1b))
 
// Multiplty is a macro used to multiply numbers in the field GF(2^8)
#define Multiply(x,y) (((y & 1) * x) ^ ((y>>1 & 1) * xtime(x)) ^ ((y>>2 & 1) * xtime(xtime(x))) ^ ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^ ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))))
 
// MixColumns function mixes the columns of the state matrix.
// The method used to multiply may be difficult to understand for the inexperienced.
// Please use the references to gain more information.
static void InvMixColumns(AES128_HANDLE* pHandle)
{
	int i;
	uint8_t a, b, c, d;
	for (i = 0; i < 4; i++)
	{
 
		a = pHandle->state[0][i];
		b = pHandle->state[1][i];
		c = pHandle->state[2][i];
		d = pHandle->state[3][i];
 
 
		pHandle->state[0][i] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
		pHandle->state[1][i] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
		pHandle->state[2][i] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
		pHandle->state[3][i] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
	}
}
 
// InvCipher is the main function that decrypts the CipherText.
static void InvCipher(AES128_HANDLE* pHandle)
{
	int i, j, round = 0;
 
	//Copy the input CipherText to state array.
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			pHandle->state[j][i] = pHandle->pDataBuff[i * 4 + j];
		}
	}
 
	// Add the First round key to the state before starting the rounds.
	AddRoundKey(pHandle, cg_Nr);
 
	// There will be cg_Nr rounds.
	// The first cg_Nr-1 rounds are identical.
	// These cg_Nr-1 rounds are executed in the loop below.
	for (round = cg_Nr - 1; round > 0; round--)
	{
		InvShiftRows(pHandle);
		InvSubBytes(pHandle);
		AddRoundKey(pHandle, round);
		InvMixColumns(pHandle);
	}
 
	// The last round is given below.
	// The MixColumns function is not here in the last round.
	InvShiftRows(pHandle);
	InvSubBytes(pHandle);
	AddRoundKey(pHandle, 0);
 
	// The decryption process is over.
	// Copy the state array to output array.
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			pHandle->pDataBuff[i * 4 + j] = pHandle->state[j][i];
		}
	}
}
 
// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
static void SubBytes(AES128_HANDLE* pHandle)
{
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			pHandle->state[i][j] = getSBoxValue(pHandle->state[i][j]);
 
		}
	}
}
 
// The ShiftRows() function shifts the rows in the state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
static void ShiftRows(AES128_HANDLE* pHandle)
{
	uint8_t temp;
 
	// Rotate first row 1 columns to left    
	temp = pHandle->state[1][0];
	pHandle->state[1][0] = pHandle->state[1][1];
	pHandle->state[1][1] = pHandle->state[1][2];
	pHandle->state[1][2] = pHandle->state[1][3];
	pHandle->state[1][3] = temp;
 
	// Rotate second row 2 columns to left    
	temp = pHandle->state[2][0];
	pHandle->state[2][0] = pHandle->state[2][2];
	pHandle->state[2][2] = temp;
 
	temp = pHandle->state[2][1];
	pHandle->state[2][1] = pHandle->state[2][3];
	pHandle->state[2][3] = temp;
 
	// Rotate third row 3 columns to left
	temp = pHandle->state[3][0];
	pHandle->state[3][0] = pHandle->state[3][3];
	pHandle->state[3][3] = pHandle->state[3][2];
	pHandle->state[3][2] = pHandle->state[3][1];
	pHandle->state[3][1] = temp;
}
 
// xtime is a macro that finds the product of {02} and the argument to xtime modulo {1b}  
#define xtime(x)   ((x<<1) ^ (((x>>7) & 1) * 0x1b))
 
// MixColumns function mixes the columns of the state matrix
static void MixColumns(AES128_HANDLE* pHandle)
{
	int i;
	uint8_t Tmp, Tm, t;
	for (i = 0; i < 4; i++)
	{
		t = pHandle->state[0][i];
		Tmp = pHandle->state[0][i] ^ pHandle->state[1][i] ^ pHandle->state[2][i] ^ pHandle->state[3][i];
		Tm = pHandle->state[0][i] ^ pHandle->state[1][i]; Tm = xtime(Tm); pHandle->state[0][i] ^= Tm ^ Tmp;
		Tm = pHandle->state[1][i] ^ pHandle->state[2][i]; Tm = xtime(Tm); pHandle->state[1][i] ^= Tm ^ Tmp;
		Tm = pHandle->state[2][i] ^ pHandle->state[3][i]; Tm = xtime(Tm); pHandle->state[2][i] ^= Tm ^ Tmp;
		Tm = pHandle->state[3][i] ^ t; Tm = xtime(Tm); pHandle->state[3][i] ^= Tm ^ Tmp;
	}
}
 
// Cipher is the main function that encrypts the PlainText.
static void Cipher(AES128_HANDLE* pHandle)
{
	int i, j, round = 0;
 
	//Copy the input PlainText to state array.
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			pHandle->state[j][i] = pHandle->pDataBuff[i * 4 + j];
		}
	}
 
	// Add the First round key to the state before starting the rounds.
	AddRoundKey(pHandle, 0);
 
	// There will be cg_Nr rounds.
	// The first cg_Nr-1 rounds are identical.
	// These cg_Nr-1 rounds are executed in the loop below.
	for (round = 1; round < cg_Nr; round++)
	{
		SubBytes(pHandle);
		ShiftRows(pHandle);
		MixColumns(pHandle);
		AddRoundKey(pHandle, round);
	}
 
	// The last round is given below.
	// The MixColumns function is not here in the last round.
	SubBytes(pHandle);
	ShiftRows(pHandle);
	AddRoundKey(pHandle, cg_Nr);
 
	// The encryption process is over.
	// Copy the state array to output array.
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			pHandle->pDataBuff[i * 4 + j] = pHandle->state[j][i];
		}
	}
}
 
 
/*************************************************************************************************************************
* 函数				:	void AES128_SetKey(AES128_HANDLE* pHandle, uint8_t Key[16])
* 功能				:	AES128设置KEY
* 参数				:	pHandle:句柄;Key:128bit key
* 返回				:	无
* 依赖				:	底层
* 作者				:	cp1300@139.com
* 时间				:	2022-01-25
* 最后修改时间 		: 	2022-01-25
* 说明				:	
*************************************************************************************************************************/
void AES128_SetKey(AES128_HANDLE* pHandle, const uint8_t Key[16])
{
	memcpy(pHandle->Key, Key, 16);
}
 
/*************************************************************************************************************************
* 函数				:	void AES128_Encrypt128bit(uint8_t pData[16], uint8_t DataSize)
* 功能				:	AES128加密128bit数据
* 参数				:	pHandle:句柄;pData：待加密数据，同时存储加密完成后的数据；DataSize：待加密数据长度1-16字节
* 返回				:	无
* 依赖				:	底层
* 作者				:	cp1300@139.com
* 时间				:	2022-01-25
* 最后修改时间 		: 	2022-01-25
* 说明				:	需要提前初始化AES加密模块KEY
*						加密的数据可以不足16字节,但是缓冲区必须是16字节
*************************************************************************************************************************/
void AES128_Encrypt128bit(AES128_HANDLE* pHandle,uint8_t pData[16], uint8_t DataSize)
{
	if (DataSize == 0) return;
	else if (DataSize > 16) DataSize = 16;
	else if (DataSize < 16)	//不足16字节,需要填充0
	{
		memset(&pData[DataSize], 0, 16 - DataSize);	//后面的填充0
	}
	pHandle->pDataBuff = pData;								//数据缓冲区直接指向pData
 
	KeyExpansion(pHandle);
	Cipher(pHandle);
}
 
/*************************************************************************************************************************
* 函数				:	uint32_t AES128_EncryptData(AES128_HANDLE* pHandle, uint8_t *pData, uint32_t DataSize)
* 功能				:	AES128加密数据
* 参数				:	pHandle:句柄;pData：待加密数据，同时存储加密完成后的数据(大小必须是16字节整数倍)；DataSize：待加密数据长度>=1
* 返回				:	加密后的数据长度
* 依赖				:	底层
* 作者				:	cp1300@139.com
* 时间				:	2022-01-25
* 最后修改时间 		: 	2022-01-25
* 说明				:	需要提前初始化AES加密模块KEY
*						数据缓冲区必须是16字节整数倍,实际数据不足部分程序会自动补0
*************************************************************************************************************************/
uint32_t AES128_EncryptData(AES128_HANDLE* pHandle, uint8_t *pData, uint32_t DataSize)
{
	uint32_t PackCount;	//16字节为一包,总包数量
	uint32_t i = 0;
	uint8_t EndPackSize;
	uint32_t len;
 
	PackCount = DataSize / 16;		//计算总包数量
	EndPackSize = DataSize % 16;	//余数
	for (i = 0; i < PackCount; i++)	//先对整数包进行验证
	{
		AES128_Encrypt128bit(pHandle, &pData[i * 16], 16);
	}
	len = PackCount * 16;
	if (EndPackSize)				//最后一包有余
	{
		AES128_Encrypt128bit(pHandle, &pData[i * 16], EndPackSize);
		len += 16;
	}
 
	return len;
}
 
 
/*************************************************************************************************************************
* 函数				:	void AES128_Decrypt128bit(AES128_HANDLE* pHandle,uint8_t  pData[16], uint8_t DataSize)
* 功能				:	AES128解密128bit数据
* 参数				:	pHandle:句柄;pData：待解密数据，同时存储解密完成后的数据；DataSize：待解密数据长度1-16字节
* 返回				:	无
* 依赖				:	底层
* 作者				:	cp1300@139.com
* 时间				:	2022-01-25
* 最后修改时间 		: 	2022-01-25
* 说明				:	需要提前初始化AES加密模块KEY
*						加密的数据可以不足16字节,但是缓冲区必须是16字节
*************************************************************************************************************************/
void AES128_Decrypt128bit(AES128_HANDLE* pHandle,uint8_t pData[16])
{
	pHandle->pDataBuff = pData;						//数据缓冲区指向临时缓冲区
	KeyExpansion(pHandle);
	InvCipher(pHandle);
}
 
 
/*************************************************************************************************************************
* 函数				:	void AES128_DecryptData(AES128_HANDLE* pHandle, uint8_t *pData, uint32_t DataSize)
* 功能				:	AES128解密数据
* 参数				:	pHandle:句柄;pData：待解密数据，同时存储解密完成后的数据(大小必须是16字节整数倍)；DataSize：待解密数据长度>=1
* 返回				:	无
* 依赖				:	底层
* 作者				:	cp1300@139.com
* 时间				:	2022-01-25
* 最后修改时间 		: 	2022-01-25
* 说明				:	需要提前初始化AES加密模块KEY
*						数据缓冲区必须是16字节整数倍,实际数据不足部分程序会自动补0
*************************************************************************************************************************/
void AES128_DecryptData(AES128_HANDLE* pHandle, uint8_t* pData, uint32_t DataSize)
{
	uint32_t PackCount;	//16字节为一包,总包数量
	uint32_t i = 0;
 
	PackCount = DataSize / 16;		//计算总包数量
	for (i = 0; i < PackCount; i++)	//先对整数包进行验证
	{
		AES128_Decrypt128bit(pHandle, &pData[i * 16]);
	}
}
