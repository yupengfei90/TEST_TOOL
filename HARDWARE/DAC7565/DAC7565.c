#include "DAC7565.h"
#include "spi.h"
#include "delay.h"

DAC7565_Seq_t DAC7565_Seq = {0};	//���͵�DAC7565�Ĵ������ݣ�
									//�����ִ��˼·����׼���������ٷ���

/* ----------------------------------------------------------------------------
    -- ���ƣ�DAC7565_Init
    -- ���ܣ���ʼ��DAC7565�������ţ�������DAΪSingle-channel update���ǽ���ģʽ
	-- ��������
	-- ���أ���
   ---------------------------------------------------------------------------- */
void DAC7565_Init(void)
{
	//DAC7565�������ų�ʼ��
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //ʹ��GPIOB��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE); //ʹ��GPIOE��ʱ��
	
	//74HCT595��RSTSEL,SYNC(-),EN(-)���Ŷ�����Ϊ�������,(-)��ʾ����Ч
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;  //�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //����GPIO
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	DAC7565_RSTSEL = 0;		//�������Ϊbinary-straight	
	DAC7565_EN = 1;
	DAC7565_SYNC = 1;
	
	//�趨���з������ݵĹ̶���ʽ
	DAC7565_Seq.bits.pd0 = 0;	//�ǽ���ģʽ 
	DAC7565_Seq.bits.ld0 = 1; 	//����ΪSingle-channel update
	DAC7565_Seq.bits.ld1 = 0;
	
	delay_us(1);
}
/*<!************************** End of DAC7565_Init() **************************/


/* ----------------------------------------------------------------------------
    -- ���ƣ�DAC7565_Output 
    -- ���ܣ���ָ��DAC7565ͨ������ض���ѹ
	-- ������channel(ͨ��),voltage(�����ѹֵ�������ѹ��Χ��0-5V)
	-- ���أ���
   ---------------------------------------------------------------------------- */
void DAC7565_Output(u8 channel, double voltage)
{
	u16 ad;
	
	//�������ͨ��
	DAC7565_Seq.data &= (~(0x3<<17));
	DAC7565_Seq.data |= ((channel & 0x3)<<17);
	
	//�������ADֵ
	ad = (u16)((voltage/(2.5*2))*4095);	//��Ϊ�ⲿ��·����2���Ŵ������������2��
										//2.5v��DAC7565����������ѹ
	DAC7565_Seq.data &= (~(0x0FFF<<4));
	DAC7565_Seq.data |= (ad<<4);
	
	DAC7565_EN = 0;
	delay_us(1);
	DAC7565_SYNC = 0;
	delay_us(1);
	MySPI_SendData((u8)(DAC7565_Seq.data >> 16));
	MySPI_SendData((u8)((DAC7565_Seq.data >> 8)&0xFF));
	MySPI_SendData((u8)(DAC7565_Seq.data&0xFF));
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);//�ȴ��������� 	
	delay_us(40);	//����ӳ�����ʱ����ΪSPI2�����ʽϵ�ʱ����������SYNC�������������ݴ������
	DAC7565_SYNC = 1;
	delay_us(1);
	DAC7565_EN = 1;
	delay_us(1);
}
/*<!************************** End of DAC7565_Output() **************************/

