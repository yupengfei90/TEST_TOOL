#ifndef _DAC7565_H
#define _DAC7565_H
#include "sys.h"
#define DAC7565_EN PBout(1)			//DAC7565 ��������Ƭѡ�ź�
#define DAC7565_RSTSEL PEout(10) 	//reset select���ţ��õ��������Ϊ����֣���ø߽���Ϊ�����Ʋ���
#define DAC7565_SYNC PEout(15)		//sync���Ų����½���ʱ����ʼ����SPI���ߵ�����
#endif 
