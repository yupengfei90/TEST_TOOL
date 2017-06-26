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
���ܣ����ݹ�װҪ������AD7328������ͨ��ģʽΪSingle-Ended��
	��Դģʽ��Normal
	������ѹ��Χ, 8��ͨ����ΪĬ�ϵ�-10v~+10v
*******************************************************************/
void AD7328_Init(void)
{
	//ADƬѡ���ų�ʼ��
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //ʹ��GPIOD��ʱ��
	
	//AD7328 CS����PB0����Ϊ�������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;  //�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //����GPIO
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	AD7328_CS = 1;
	
	AD_ControlReg_Serial.bits.write = 1;
	AD_ControlReg_Serial.bits.register_select1 = 0;	//ѡ��AD7328�ڲ��Ŀ��ƼĴ���
	AD_ControlReg_Serial.bits.register_select2 = 0;
	AD_ControlReg_Serial.bits.mode0 = 0;			//��Ϊ8 Single-Endedģʽ����ͨ������
	AD_ControlReg_Serial.bits.mode1 = 0;
	AD_ControlReg_Serial.bits.pm0 = 0;				//��Դģʽ��ΪNormal,�ϵ��һֱ����
	AD_ControlReg_Serial.bits.pm1 = 0;
	AD_ControlReg_Serial.bits.coding = 1;			//��straight binary���
	AD_ControlReg_Serial.bits.ref = 1;				//ʹ��internal reference(2.5v)
	AD_ControlReg_Serial.bits.seq1 = 0;				//��ʹ��Channel Sequencer
	AD_ControlReg_Serial.bits.seq2 = 0;
	AD_ControlReg_Serial.bits.weak = 0;				//DOUT�ߴ�����Ϸ���three-state״̬(����״̬)
}

/******************************************************************
���ܣ���ȡָ��ͨ����ȡ����ADֵ
������channel��Ҫ������ͨ��(0-7)
���أ�����ģ���źŲɼ�ת�����ADֵ
*******************************************************************/
//u16 AD7328_ChannelRead(u8 channel)
//{
//	u16 ConversionSerial = 0;
//	u16 ConversionAD = 0;
//	
//	//����ͨ��
//	AD_ControlReg_Serial.data |= ((channel & 0x7) << 10);	
//	delay_us(1);
//	AD7328_CS = 0;
//	delay_us(1);
//	SPI2_ReadWriteByte((u8)(AD_ControlReg_Serial.data >> 8));	//�ȴ���߰�λ
//	SPI2_ReadWriteByte((u8)(AD_ControlReg_Serial.data & 0xFF));	//����Ͱ�λ
//	delay_us(55);
//	AD7328_CS = 1;
//	delay_us(1);
//	
//	//��ȡת�����
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
	���ƣ�AD7328_ChannelSelect
	���ܣ�ѡ��AD7328�����ͨ��
	������channel,ѡ���ͨ����0-7
  ============================================================================*/
void AD7328_ChannelSelect(u8 channel)
{
	AD_ControlReg_Serial.data &= (~(0x7 << 10));
	AD_ControlReg_Serial.data |= ((channel & 0x7) << 10);	
	
	AD7328_CS = 0;
	delay_us(1);
	MySPI_SendData((u8)(AD_ControlReg_Serial.data >> 8));	//�ȴ���߰�λ
	MySPI_SendData((u8)(AD_ControlReg_Serial.data & 0xFF));	//����Ͱ�λ
	delay_us(55);
	AD7328_CS = 1;
	delay_us(1);
}
/*****************************End of AD7328_ChannelSelect*****************************/


/*============================================================================
	���ƣ�AD7328_ReadAD
	���ܣ���ȡǰһ������AD7328���ص�16λ��������
	���أ���16λ������������ȡ��12λADֵ
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
//	MySPI_SendData((u8)(AD_ControlReg_Serial.data >> 8));	//�ȴ���߰�λ
	ConversionSerial |= (MySPI_ReceiveData() << 8);
	MySPI_SendData(0x05);
//	MySPI_SendData((u8)(AD_ControlReg_Serial.data & 0xFF));	//����Ͱ�λ
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
	���ƣ�AD7328_Sample
	���ܣ���ȡָ����ADֵ
	������select,ָ��AD�ڵĺ꣬���AD7328.h�е�˵��
	���أ���16λ������������ȡ��12λADֵ
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
	
	printf("adSerial = 0x%x��vol = %.2fv\r\n",ad,((ad&0xFFF)/4095.0)*10);
	
	return ad;
}
/*****************************End of AD7328_Sample*****************************/



/******************************************************************
���ܣ���Ϊ8 Single-Endedģʽ��internal reference,
	  ��ʹ��Channel Sequencer,��straight binary���
*******************************************************************/
//static void control_register_set()
//{

//	
//	delay_us(1);
//	AD7328_CS = 0;
//	delay_us(1);
//	MySPI_SendData((u8)(AD_ControlReg_Serial.data >> 8));	//�ȴ���߰�λ
//	MySPI_SendData((u8)(AD_ControlReg_Serial.data & 0xFF));	//����Ͱ�λ
//	delay_us(1);
//	AD7328_CS = 1;
//	delay_us(500);				//��ʱ500us,�ȴ�internal reference���ݻ����ȶ�
//}

/******************************************************************
���ƣ�range_register_set
���ܣ�ͨ��0-7������Χ����Ϊ0-10V
*******************************************************************/
//void range_register_set(void)
//{
//	delay_us(1);
//	AD7328_CS = 0;
//	delay_us(1);
//	MySPI_SendData((u8)(CHANNEL0_3_SETRANGE0_10 >> 8));	//ͨ��0-3,������ѹ��Χ��Ϊ0-10V
//	MySPI_SendData((u8)(CHANNEL0_3_SETRANGE0_10 & 0xFF));
//	delay_us(55);
//	AD7328_CS = 1;
//	delay_us(1);
//	
//	AD7328_CS = 0;
//	delay_us(1);
//	MySPI_SendData((u8)(CHANNEL4_7_SETRANGE0_10 >>8));	//ͨ��4-7,������ѹ��Χ��Ϊ0-10V
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



