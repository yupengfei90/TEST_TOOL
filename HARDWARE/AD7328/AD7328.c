/*
	AD7328.c
*/
#include "AD7328.h"
#include "spi.h"
#include "delay.h"

AD_ControlReg_Serial_t AD_ControlReg_Serial = {0};
static void range_register_set(void);
static void control_register_set(void);

/******************************************************************
功能：根据工装要求，设置AD7328的输入通道模式为Single-Ended，
	电源模式，Normal
	采样电压范围, 8个通道均为0-10V
*******************************************************************/
void AD7328_Init(void)
{
	//AD片选引脚初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //使能GPIOD的时钟
	
	//74HCT595的DS,SHCP,STCP,MR引脚都设置为输出引脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;  //下拉输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //高速GPIO
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	control_register_set();
	range_register_set();	//通道0-7,采样电压范围都设为0-10V
}

/******************************************************************
功能：读取指定通道采取到的AD值
参数：channel，要采样的通道
返回：返回模拟信号采集转换后的AD值
*******************************************************************/
u16 AD7328_ChannelRead(u8 channel)
{
	u16 Conversion = 0;
	u16 ConversionAD = 0;
	
	//设置通道
	AD_ControlReg_Serial.data |= (channel << 10);	
	AD7328_CS = 0;
	SPI2_ReadWriteByte((u8)(AD_ControlReg_Serial.data >> 8));	//先传输高八位
	SPI2_ReadWriteByte((u8)(AD_ControlReg_Serial.data & 0xFF));	//传输低八位
	AD7328_CS = 1;
	delay_us(1);
	//读取转换结果
	AD7328_CS = 0;
	Conversion |= (SPI2_ReadWriteByte(READBYTE) << 8);
	Conversion |= SPI2_ReadWriteByte(READBYTE);
	ConversionAD = (Conversion & 0xFFF);
	AD7328_CS = 1;
	delay_us(1);
	
	return ConversionAD;
}

/******************************************************************
功能：设为8 Single-Ended模式，internal reference,
	  不使用Channel Sequencer,以straight binary输出
*******************************************************************/
static void control_register_set()
{
	AD_ControlReg_Serial.bits.write = 1;
	AD_ControlReg_Serial.bits.register_select1 = 0;	//选择AD7328内部的控制寄存器
	AD_ControlReg_Serial.bits.register_select2 = 0;
	AD_ControlReg_Serial.bits.mode0 = 0;			//设为8 Single-Ended模式，单通道采样
	AD_ControlReg_Serial.bits.mode1 = 0;
	AD_ControlReg_Serial.bits.pm0 = 0;				//电源模式设为Normal,上电后一直工作
	AD_ControlReg_Serial.bits.pm1 = 0;
	AD_ControlReg_Serial.bits.coding = 0;			//以straight binary输出
	AD_ControlReg_Serial.bits.ref = 1;				//使用internal reference(2.5v)
	AD_ControlReg_Serial.bits.seq1 = 0;				//不使用Channel Sequencer
	AD_ControlReg_Serial.bits.seq2 = 0;
	AD_ControlReg_Serial.bits.weak = 0;				//DOUT线传输完毕返回three-state状态(高阻状态)
	
	AD7328_CS = 0;
	SPI2_ReadWriteByte((u8)(AD_ControlReg_Serial.data >> 8));	//先传输高八位
	SPI2_ReadWriteByte((u8)(AD_ControlReg_Serial.data & 0xFF));	//传输低八位
	AD7328_CS = 1;
	delay_us(500);				//延时500us,等待internal reference电容缓冲稳定
}

/******************************************************************
功能：通道0-7采样范围均设为0-10V
*******************************************************************/
static void range_register_set(void)
{
	AD7328_CS = 0;
	SPI2_ReadWriteByte((u8)(CHANNEL0_3_SETRANGE0_10 >> 8));	//通道0-3,采样电压范围设为0-10V
	SPI2_ReadWriteByte((u8)(CHANNEL0_3_SETRANGE0_10 & 0xFF));
	SPI2_ReadWriteByte((u8)(CHANNEL4_7_SETRANGE0_10 >>8));	//通道4-7,采样电压范围设为0-10V
	SPI2_ReadWriteByte((u8)(CHANNEL4_7_SETRANGE0_10 & 0xFF));
	AD7328_CS = 1;
	delay_us(1);
}



