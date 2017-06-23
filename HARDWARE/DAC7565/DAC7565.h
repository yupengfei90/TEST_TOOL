#ifndef _DAC7565_H
#define _DAC7565_H
#include "sys.h"

#define DAC7565_EN PBout(1)			//DAC7565 ��������Ƭѡ�ź�
#define DAC7565_RSTSEL PEout(10) 	//reset select���ţ��õ��������Ϊ����֣���ø߽���Ϊ�����Ʋ���
#define DAC7565_SYNC PEout(15)		//sync���Ų����½���ʱ����ʼ����SPI���ߵ�����
typedef union{
	u32 data;
	struct{
		u32 db0:1;	//db0-db3 don't care bits
		u32 db1:1;
		u32 db2:1;
		u32 db3:1;
		u32 d0:1;	//d0-d11Ϊ����λ
		u32 d1:1;
		u32 d2:1;
		u32 d3:1;
		u32 d4:1;
		u32 d5:1;
		u32 d6:1;
		u32 d7:1;
		u32 d8:1;
		u32 d9:1;
		u32 d10:1;
		u32 d11:1;
		u32 pd0:1;	//����ģʽѡ��1Ϊ���ܣ�0Ϊ����
		u32 dac_select0:1;	//���ͨ��ѡ��λ0
		u32 dac_select1:1;	//���ͨ��ѡ��λ1
		u32 db19:1;	//�̶�Ϊ0
		u32 ld0:1;	//����ģ�����
		u32 ld1:1;	//����ģ�����
		u32 db22:1;	//�̶�Ϊ0
		u32 db23:1; //�̶�Ϊ0
	}bits;
}DAC7565_Seq_t;	//DAC7565���յ�24λ�������ݵĸ�ʽ

/* -------------------------------------------
	--Global Function
   ------------------------------------------- */
void DAC7565_Init(void);
void DAC7565_Output(u8 channel, double voltage);

#endif 
