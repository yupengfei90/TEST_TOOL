#ifndef _AD7328_H
#define _AD7328_H
#include "sys.h"

#define CHANNEL0_3_SETRANGE0_10 0xBFE0	//ͨ��0-3������ѹ��Χ��Ϊ0-10V�Ĵ�������
#define CHANNEL4_7_SETRANGE0_10 0xDFE0	//ͨ��4-7������ѹ��Χ��Ϊ0-10V�Ĵ�������
#define READBYTE 0x55
/*****************************************************************
	AD7328���ݸ�ʽ
******************************************************************/
typedef union{
	u16 data;
	struct {
		u16 lsb:1;
		u16 weak:1;
		u16 seq2:1;
		u16 seq1:1;
		u16 ref:1;
		u16 coding:1;
		u16 pm0:1;
		u16 pm1:1;
		u16 mode0:1;
		u16 mode1:1;
		u16 addr0:1;
		u16 addr1:1;
		u16 addr2:1;
		u16 register_select2:1;
		u16 register_select1:1;
		u16 write:1;
	}bits;
}AD_ControlReg_Serial_t;


/*****************************************************************
Externel function
******************************************************************/
void AD7328_Init(void);
u16 AD7328_ChannelRead(u8 channel);

#endif
