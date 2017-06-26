#ifndef _AD7328_H
#define _AD7328_H
#include "sys.h"

#define AD7328_CS PBout(0)
#define CHANNEL0_3_SETRANGE0_10 0xBFE0	//通道0-3采样电压范围设为0-10V的串行命令
#define CHANNEL4_7_SETRANGE0_10 0xDFE0	//通道4-7采样电压范围设为0-10V的串行命令


/*===============================================================
	要采集的AD口的宏定义
  ===============================================================*/
//Andata0-是为原理图上的标号
#define REQ_GATHER_AD3 0x0501	//高字节表示AD7328通道选择，低八位表示HCF4051通道选择
								//0x0501的含义为AD7328选择第5通道作为输入，HCF4051选择第1通道作为输出
								//此时选中的正是REQ_GATHER_AD3口，下同不再赘述
#define REQ_GATHER_AD1 0x0502
#define REQ_GATHER_AD4 0x0500
#define REQ_GATHER_AD2 0x0503
#define REQ_GATHER_AD8 0x0505
#define REQ_GATHER_AD7 0x0507
#define REQ_GATHER_AD6 0x0506
#define REQ_GATHER_AD5 0x0504

//Andata1-是为原理图上的标号
#define SIG_Trig_Hig_AD6 0x04001
#define SIG_Trig_Hig_AD7 0x04002
#define SIG_Trig_Hig_AD5 0x04000
#define SIG_Trig_Hig_AD4 0x04003
#define SIG_Trig_Hig_AD8 0x04005
#define SIG_Sensor_AD8 0x0407
#define SIG_Sensor_AD9 0x0406
#define REQ_Gather_AD0 0x0404

//Andata2-是为原理图上的标号
#define Location_DC_motor_AD2 0x0101
#define Location_Step_AD1 0x0102
#define Location_Step_AD2 0x0100
#define Location_DC_motor_AD3 0x0103
#define Location_DC_motor_AD1 0x0105
#define Location_Step_AD0 0x0107
#define Location_DC_motor_AD0 0x0106
#define DC_Motor_5V_AD 0x0104

//Andata3-是为原理图上的标号
#define Blower_FB_AD 0x0601
#define P12V_VOL_AD 0x0602
#define SIG_Trig_Low_AD0 0x0600
#define SIG_Trig_Low_AD1 0x0603
#define Blower_Cur_AD 0x0605
#define SIG_Trig_Hig_Cur_AD 0x0607
#define SIG_Trig_Low_Cur_AD 0x0606
#define Motor_Step_Cur_AD 0x0604

//Andata4-是为原理图上的标号
#define Sig_Trig_Hig_AD2 0x0301
#define Sig_Trig_Hig_AD1 0x0302
#define Power_Vol_AD	0x0300
#define KL30_Vol_AD	0x0303
#define SIG_Trig_Hig_AD3 0x0305
#define SIG_Trig_Hig_AD0 0x0307
#define SIG_Trig_Low_AD3 0x0306
#define SIG_Trig_Low_AD2 0x0304

//Andata5-是为原理图上的标号
#define KL30_Cur_AD	0x0001
#define POWER_Cur_AD	0x0002
#define KL15_Vol_AD	0x0000
#define Reserved_AD0003  0x0003
#define KL15_Cur_AD	0x0005
#define Location_Step_AD4 0x0007
#define Location_DC_motor_AD4 0x0006
#define Location_Step_AD3 0x0004

//Andata6-是为原理图上的标号
#define SIG_Sensor_AD2 0x0201
#define SIG_Trig_Low_AD5 0x0202
#define SIG_Sensor_AD1 0x0200
#define SIG_Trig_Low_AD6 0x0203
#define SIG_Sensor_AD7 0x0205
#define SIG_Trig_Low_AD4 0x0207
#define SIG_Sensor_AD6 0x0206
#define SIG_Sensor_AD3 0x0204

//Andata7-是为原理图上的标号
#define Reserved_AD0701  0x0701
#define SIG_Trig_Low_AD7 0x0702
#define Reserved_AD0700  0x0700
#define Reserved_AD0703  0x0703
#define SIG_Sensor_AD4 0x0705
#define SIG_Sensor_AD5 0x0707
#define SIG_Trig_Low_AD8 0x0706
#define SIG_Sensor_AD0 0x0704

/*****************************************************************
	AD7328数据格式
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
u16 AD7328_Sample(u16 select);

void AD7328_ChannelSelect(u8 channel);
u16 AD7328_ReadAD(void);	

#endif
