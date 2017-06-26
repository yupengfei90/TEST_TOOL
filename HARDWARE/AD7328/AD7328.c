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
	
	AD7328_CS = 1;
	
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
}

/******************************************************************
功能：读取指定通道采取到的AD值
参数：channel，要采样的通道(0-7)
返回：返回模拟信号采集转换后的AD值
*******************************************************************/
//u16 AD7328_ChannelRead(u8 channel)
//{
//	u16 ConversionSerial = 0;
//	u16 ConversionAD = 0;
//	
//	//设置通道
//	AD_ControlReg_Serial.data |= ((channel & 0x7) << 10);	
//	delay_us(1);
//	AD7328_CS = 0;
//	delay_us(1);
//	SPI2_ReadWriteByte((u8)(AD_ControlReg_Serial.data >> 8));	//先传输高八位
//	SPI2_ReadWriteByte((u8)(AD_ControlReg_Serial.data & 0xFF));	//传输低八位
//	delay_us(55);
//	AD7328_CS = 1;
//	delay_us(1);
//	
//	//读取转换结果
//	delay_us(1);
//	AD7328_CS = 0;
//	delay_us(1);
//	ConversionSerial |= (SPI2_ReadWriteByte((u8)0x55) << 8);
//	ConversionSerial |= SPI2_ReadWriteByte((u8)0x55);
//	delay_us(55);
//	ConversionAD = (ConversionSerial & 0xFFF);
//	AD7328_CS = 1;
//	delay_us(1);
//	
//	return ConversionAD;
//}

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
	返回：从16位串行数据中提取的12位AD值
  ============================================================================*/
u16 AD7328_ReadAD(void)
{
	u16 ConversionSerial = 0;
	u16	ConversionAD = 0;
	
//	delay_us(1);
//	AD7328_CS = 0;
//	delay_us(1);
//	MySPI_SendData(0x05);
//	MySPI_SendData(0x05);
//	//ConversionSerial |= (SPI2_ReadWriteByte(0x55)<<8);
//	//ConversionSerial |= SPI2_ReadWriteByte(0x55);
//	delay_us(55);
//	AD7328_CS = 1;
//	delay_us(1);
	
	delay_us(1);
	AD7328_CS = 0;
	delay_us(1);
	MySPI_SendData(0x05);
//	MySPI_SendData((u8)(AD_ControlReg_Serial.data >> 8));	//先传输高八位
	ConversionSerial |= (MySPI_ReceiveData() << 8);
	MySPI_SendData(0x05);
//	MySPI_SendData((u8)(AD_ControlReg_Serial.data & 0xFF));	//传输低八位
	ConversionSerial |= (MySPI_ReceiveData() & 0xFF);
	//ConversionSerial |= (SPI2_ReadWriteByte(0x55)<<8);
	//ConversionSerial |= SPI2_ReadWriteByte(0x55);
	ConversionAD = (ConversionSerial & 0xFFF);
	delay_us(55);
	AD7328_CS = 1;
	delay_us(1);
	
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
	u16 ad;
	u8 AD7328_Channel, HCF4051_Channel;
	
	AD7328_Channel = (u8)(select >> 8);
	HCF4051_Channel = (u8)(select & 0xFF);
	HCF4051_ChannelSelect(HCF4051_Channel);
	AD7328_ChannelSelect(AD7328_Channel);
	
	delay_ms(10);
	ad = AD7328_ReadAD();
	
	printf("adSerial = 0x%x，vol = %.2fv\r\n",ad,((ad&0xFFF)/4095.0)*10);
	
	return ad;
}
/*****************************End of AD7328_Sample*****************************/



/******************************************************************
功能：设为8 Single-Ended模式，internal reference,
	  不使用Channel Sequencer,以straight binary输出
*******************************************************************/
//static void control_register_set()
//{

//	
//	delay_us(1);
//	AD7328_CS = 0;
//	delay_us(1);
//	MySPI_SendData((u8)(AD_ControlReg_Serial.data >> 8));	//先传输高八位
//	MySPI_SendData((u8)(AD_ControlReg_Serial.data & 0xFF));	//传输低八位
//	delay_us(1);
//	AD7328_CS = 1;
//	delay_us(500);				//延时500us,等待internal reference电容缓冲稳定
//}

/******************************************************************
名称：range_register_set
功能：通道0-7采样范围均设为0-10V
*******************************************************************/
//void range_register_set(void)
//{
//	delay_us(1);
//	AD7328_CS = 0;
//	delay_us(1);
//	MySPI_SendData((u8)(CHANNEL0_3_SETRANGE0_10 >> 8));	//通道0-3,采样电压范围设为0-10V
//	MySPI_SendData((u8)(CHANNEL0_3_SETRANGE0_10 & 0xFF));
//	delay_us(55);
//	AD7328_CS = 1;
//	delay_us(1);
//	
//	AD7328_CS = 0;
//	delay_us(1);
//	MySPI_SendData((u8)(CHANNEL4_7_SETRANGE0_10 >>8));	//通道4-7,采样电压范围设为0-10V
//	MySPI_SendData((u8)(CHANNEL4_7_SETRANGE0_10 & 0xFF));
//	delay_us(55);
//	AD7328_CS = 1;
//	delay_us(1);
//}
/*****************************End of range_register_set*****************************/



//void SPI_ReadTest()
//{
//	u8 ad1,ad2;
//	
//	AD7328_CS = 0;
//	delay_us(1);
//	ad1 = SPI2_ReadWriteByte(0xaa);
//	delay_us(1);
//	AD7328_CS = 1;
//	delay_us(1);
//	
//	AD7328_CS = 0;
//	delay_us(1);
//	ad2 = SPI2_ReadWriteByte(0xc0);
//	delay_us(1);
//	AD7328_CS = 1;
//	delay_us(1);
//	
//	printf("ad1 = 0x%x\r\n",ad1);
//	printf("ad2 = 0x%x\r\n",ad2);
//}



