/*
	AD7328.c
*/
#include "AD7328.h"
#include "HCF4051.h"
#include "spi.h"
#include "delay.h"
#include "usart.h"

AD_ControlReg_Serial_t AD_ControlReg_Serial = {0};

/******************************************************************
功能：根据工装要求，设置AD7328的输入通道模式为Single-Ended，
	电源模式，Normal
	采样电压范围, 8个通道均为默认的-10v~+10v
*******************************************************************/
void AD7328_Init(void)
{
	//AD片选引脚初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //使能GPIOD的时钟
	
	//AD7328 CS引脚PB0设置为输出引脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;  //上拉输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //高速GPIO
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//将发往AD内部寄存器Control register的16位串行数据的某些位设为固定值
	AD_ControlReg_Serial.bits.write = 1;
	AD_ControlReg_Serial.bits.register_select1 = 0;	//选择AD7328内部的控制寄存器
	AD_ControlReg_Serial.bits.register_select2 = 0;
	AD_ControlReg_Serial.bits.mode0 = 0;			//设为8 Single-Ended模式，单通道采样
	AD_ControlReg_Serial.bits.mode1 = 0;
	AD_ControlReg_Serial.bits.pm0 = 0;				//电源模式设为Normal,上电后一直工作
	AD_ControlReg_Serial.bits.pm1 = 0;
	AD_ControlReg_Serial.bits.coding = 1;			//以straight binary输出
	AD_ControlReg_Serial.bits.ref = 1;				//使用internal reference(2.5v)
	AD_ControlReg_Serial.bits.seq1 = 0;				//不使用Channel Sequencer
	AD_ControlReg_Serial.bits.seq2 = 0;
	AD_ControlReg_Serial.bits.weak = 0;				//DOUT线传输完毕返回three-state状态(高阻状态)
	
	AD7328_CS = 1;
	delay_us(1);
}


/*============================================================================
	名称：AD7328_ChannelSelect
	功能：选择AD7328的输出通道
	参数：channel,选择的通道，0-7
  ============================================================================*/
void AD7328_ChannelSelect(u8 channel)
{
	AD_ControlReg_Serial.data &= (~(0x7 << 10));
	AD_ControlReg_Serial.data |= ((channel & 0x7) << 10);	
	
	AD7328_CS = 0;
	delay_us(1);
	MySPI_SendData((u8)(AD_ControlReg_Serial.data >> 8));	//先传输高八位
	MySPI_SendData((u8)(AD_ControlReg_Serial.data & 0xFF));	//传输低八位
	delay_us(55);
	AD7328_CS = 1;
	delay_us(1);
}
/*****************************End of AD7328_ChannelSelect*****************************/


/*============================================================================
	名称：AD7328_ReadAD
	功能：读取前一配置下AD7328返回的16位串行数据
	返回：从AD返回的16位串行数据，前三位表示通道，第4位表示符号，后12位为转化所得AD值
	备注：因为采样电压范围设为-10-10V,所以采样电压如略低于GND，就会出现负的满量程显示,
		  -10 - 0的AD我猜想是递增的，从0-FFF
		  所以这里需要对返回的串行数据中的符号位进行判断，若符号位为0，则表示负电压
		  该工装的没有负电压，所以若符号位为0，则数据位全部置0，回传最低电压0v
  ============================================================================*/
u16 AD7328_ReadAD(void)
{
	u16 ConversionSerial = 0;
	u16 bit_4;	//符号位，1代表正电压，0代表负电压
	
	delay_us(1);
	AD7328_CS = 0;
	delay_us(1);
	MySPI_SendData(0x05);
	ConversionSerial |= (MySPI_ReceiveData() << 8);
	MySPI_SendData(0x05);
	ConversionSerial |= (MySPI_ReceiveData() & 0xFF);
	delay_us(55);
	AD7328_CS = 1;
	delay_us(1);
	
	//符号位判断，根据符号位对AD数据进行处理
	bit_4 = (ConversionSerial & 0x1000);
	if(!bit_4)
		ConversionSerial &= 0xF000;
	
	return ConversionSerial;
}
/*****************************End of AD7328_ReadAD*****************************/


/*============================================================================
	名称：AD7328_Sample
	功能：读取指定的AD值
	参数：select,指定AD口的宏，详见AD7328.h中的说明
	返回：从16位串行数据中提取的12位AD值
  ============================================================================*/
u16 AD7328_Sample(u16 select)
{
	u16 adSeria,ad;
	u8 AD7328_Channel, HCF4051_Channel;
	
	AD7328_Channel = (u8)(select >> 8);
	HCF4051_Channel = (u8)(select & 0xFF);
	HCF4051_ChannelSelect(HCF4051_Channel);
	AD7328_ChannelSelect(AD7328_Channel);
	
	delay_ms(10);	//等待通道切换稳定和AD转化完成
	adSeria = AD7328_ReadAD();
	ad = adSeria&0xFFF;
	
	printf("adSerial = 0x%x，vol = %.2fv\r\n ",adSeria,(ad/4095.0)*10);
	
	return ad;
}
/*****************************End of AD7328_Sample*****************************/

