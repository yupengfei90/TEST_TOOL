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
	
	//������AD�ڲ��Ĵ���Control register��16λ�������ݵ�ĳЩλ��Ϊ�̶�ֵ
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
	
	AD7328_CS = 1;
	delay_us(1);
}


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
	���أ���AD���ص�16λ�������ݣ�ǰ��λ��ʾͨ������4λ��ʾ���ţ���12λΪת������ADֵ
	��ע����Ϊ������ѹ��Χ��Ϊ-10-10V,���Բ�����ѹ���Ե���GND���ͻ���ָ�����������ʾ,
		  -10 - 0��AD�Ҳ����ǵ����ģ���0-FFF
		  ����������Ҫ�Է��صĴ��������еķ���λ�����жϣ�������λΪ0�����ʾ����ѹ
		  �ù�װ��û�и���ѹ������������λΪ0��������λȫ����0���ش���͵�ѹ0v
  ============================================================================*/
u16 AD7328_ReadAD(void)
{
	u16 ConversionSerial = 0;
	u16 bit_4;	//����λ��1��������ѹ��0������ѹ
	
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
	
	//����λ�жϣ����ݷ���λ��AD���ݽ��д���
	bit_4 = (ConversionSerial & 0x1000);
	if(!bit_4)
		ConversionSerial &= 0xF000;
	
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
	u16 adSeria,ad;
	u8 AD7328_Channel, HCF4051_Channel;
	
	AD7328_Channel = (u8)(select >> 8);
	HCF4051_Channel = (u8)(select & 0xFF);
	HCF4051_ChannelSelect(HCF4051_Channel);
	AD7328_ChannelSelect(AD7328_Channel);
	
	delay_ms(10);	//�ȴ�ͨ���л��ȶ���ADת�����
	adSeria = AD7328_ReadAD();
	ad = adSeria&0xFFF;
	
	printf("adSerial = 0x%x��vol = %.2fv\r\n ",adSeria,(ad/4095.0)*10);
	
	return ad;
}
/*****************************End of AD7328_Sample*****************************/

