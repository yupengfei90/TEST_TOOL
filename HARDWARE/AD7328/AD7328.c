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
���ܣ����ݹ�װҪ������AD7328������ͨ��ģʽΪSingle-Ended��
	��Դģʽ��Normal
	������ѹ��Χ, 8��ͨ����Ϊ0-10V
*******************************************************************/
void AD7328_Init(void)
{
	//ADƬѡ���ų�ʼ��
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //ʹ��GPIOD��ʱ��
	
	//74HCT595��DS,SHCP,STCP,MR���Ŷ�����Ϊ�������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;  //�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //����GPIO
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	control_register_set();
	range_register_set();	//ͨ��0-7,������ѹ��Χ����Ϊ0-10V
}

/******************************************************************
���ܣ���ȡָ��ͨ����ȡ����ADֵ
������channel��Ҫ������ͨ��
���أ�����ģ���źŲɼ�ת�����ADֵ
*******************************************************************/
u16 AD7328_ChannelRead(u8 channel)
{
	u16 Conversion = 0;
	u16 ConversionAD = 0;
	
	//����ͨ��
	AD_ControlReg_Serial.data |= (channel << 10);	
	AD7328_CS = 0;
	SPI2_ReadWriteByte((u8)(AD_ControlReg_Serial.data >> 8));	//�ȴ���߰�λ
	SPI2_ReadWriteByte((u8)(AD_ControlReg_Serial.data & 0xFF));	//����Ͱ�λ
	AD7328_CS = 1;
	delay_us(1);
	//��ȡת�����
	AD7328_CS = 0;
	Conversion |= (SPI2_ReadWriteByte(READBYTE) << 8);
	Conversion |= SPI2_ReadWriteByte(READBYTE);
	ConversionAD = (Conversion & 0xFFF);
	AD7328_CS = 1;
	delay_us(1);
	
	return ConversionAD;
}

/******************************************************************
���ܣ���Ϊ8 Single-Endedģʽ��internal reference,
	  ��ʹ��Channel Sequencer,��straight binary���
*******************************************************************/
static void control_register_set()
{
	AD_ControlReg_Serial.bits.write = 1;
	AD_ControlReg_Serial.bits.register_select1 = 0;	//ѡ��AD7328�ڲ��Ŀ��ƼĴ���
	AD_ControlReg_Serial.bits.register_select2 = 0;
	AD_ControlReg_Serial.bits.mode0 = 0;			//��Ϊ8 Single-Endedģʽ����ͨ������
	AD_ControlReg_Serial.bits.mode1 = 0;
	AD_ControlReg_Serial.bits.pm0 = 0;				//��Դģʽ��ΪNormal,�ϵ��һֱ����
	AD_ControlReg_Serial.bits.pm1 = 0;
	AD_ControlReg_Serial.bits.coding = 0;			//��straight binary���
	AD_ControlReg_Serial.bits.ref = 1;				//ʹ��internal reference(2.5v)
	AD_ControlReg_Serial.bits.seq1 = 0;				//��ʹ��Channel Sequencer
	AD_ControlReg_Serial.bits.seq2 = 0;
	AD_ControlReg_Serial.bits.weak = 0;				//DOUT�ߴ�����Ϸ���three-state״̬(����״̬)
	
	AD7328_CS = 0;
	SPI2_ReadWriteByte((u8)(AD_ControlReg_Serial.data >> 8));	//�ȴ���߰�λ
	SPI2_ReadWriteByte((u8)(AD_ControlReg_Serial.data & 0xFF));	//����Ͱ�λ
	AD7328_CS = 1;
	delay_us(500);				//��ʱ500us,�ȴ�internal reference���ݻ����ȶ�
}

/******************************************************************
���ܣ�ͨ��0-7������Χ����Ϊ0-10V
*******************************************************************/
static void range_register_set(void)
{
	AD7328_CS = 0;
	SPI2_ReadWriteByte((u8)(CHANNEL0_3_SETRANGE0_10 >> 8));	//ͨ��0-3,������ѹ��Χ��Ϊ0-10V
	SPI2_ReadWriteByte((u8)(CHANNEL0_3_SETRANGE0_10 & 0xFF));
	SPI2_ReadWriteByte((u8)(CHANNEL4_7_SETRANGE0_10 >>8));	//ͨ��4-7,������ѹ��Χ��Ϊ0-10V
	SPI2_ReadWriteByte((u8)(CHANNEL4_7_SETRANGE0_10 & 0xFF));
	AD7328_CS = 1;
	delay_us(1);
}



