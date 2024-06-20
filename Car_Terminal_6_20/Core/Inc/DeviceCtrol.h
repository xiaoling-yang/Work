#ifndef __DEVICECTROL_H
#define __DEVICECTROL_H
#include "main.h"
// 0 关闭  1打开
//开关可以控制是否触发外部中断

void AS608_Ctrol(uint8_t state);//指纹模块
void MQ3_Ctrol(uint8_t state);//控制酒精传感器是否开始采集



#endif