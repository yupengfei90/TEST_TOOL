#ifndef _74HC595_H
#define _74HC595_H
#include "sys.h"
//控制引脚
#define DS PEout(11)
#define STCP PEout(13)
#define SHCP PEout(12)
#define MR	 PEout(14)

//各开关对应的32位发送值,低位先传
#define CleanOverCur (u32)(0x01 << 0)
#define SWSIGSensor4 (u32)(0x01 << 4)
#define SWSIGSensor5 (u32)(0x01 << 5)
#define SWSIGSensor1 (u32)(0x01 << 6)
#define SWSIGSensor0 (u32)(0x01 << 7)

#define SWSIGSensor2 (u32)(0x01 << 16)
#define SWSIGSensor7 (u32)(0x01 << 17)
#define SWSIGSensor6 (u32)(0x01 << 18)
#define SWSIGSensor3 (u32)(0x01 << 19)
#define DarkCurEn (u32)(0x01 << 20)
#define SIGTrigHigEN2 (u32)(0x01 << 21)
#define SIGTrigHigEN3 (u32)(0x01 << 22)
#define SIGTrigHigEN1 (u32)(0x01 << 23)

#define SIGTrigHigEN0 (u32)(0x01 << 24)
#define SIGTrigLowEN3 (u32)(0x01 << 25)
#define SIGTrigLowEN2 (u32)(0x01 << 26)
#define SIGTrigLowEN1 (u32)(0x01 << 27)
#define SIGTrigLowEN0 (u32)(0x01 << 28)
#define HIGDriveCurEn (u32)(0x01 << 29)
#define BlowerRelayCtrlSW (u32)(0x01 << 30)
#define LowDriveCurEn (u32)(0x01 << 31)

//Global Function
void Init_74HC595(void);
void SW_Open(u32 state);

#endif 
