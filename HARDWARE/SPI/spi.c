/*
	spi.c
*/
#include "spi.h"

//spi2���ùܽ�����
void SPI_GPIOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);  //ʹ��GPIOBʱ��
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  //����Ϊ���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_SPI2);  //����ΪSPI2
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);
}



//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ 	
void SPI2_Init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
	SPI_GPIOConfig();
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);  //��SPI2ʱ��
    
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;   	//����Ϊ����ģʽ
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;   //8λ֡�ṹ
    SPI_InitStructure.SPI_CPOL  = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//Ԥ�貨���ʣ�����SPI2_SetSpeed�и���
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2,&SPI_InitStructure);
    SPI_Cmd(SPI2,ENABLE);	//ʹ��SPI2
	MySPI_SendData(0xFF);
}


//SPI2�ٶ����ú���
//SPI�ٶ�=fAPB1/��Ƶϵ��
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//fAPB1ʱ��һ��Ϊ42Mhz��
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
	SPI2->CR1&=0XFFC7;//λ3-5���㣬�������ò�����
	SPI2->CR1|=SPI_BaudRatePrescaler;	//����SPI2�ٶ� 
	SPI_Cmd(SPI2,ENABLE); //ʹ��SPI1
} 


void MySPI_SendData(char da)
{
    SPI2->DR;	//���֮ǰ���յ�������
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET){;}
    SPI_SendData(SPI2,da);
}

u8 MySPI_ReceiveData(void)
{
    while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE)==RESET);
    return SPI_ReceiveData(SPI2);
}

//SPI2 ��д1���ֽڣ�spi2����
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI2_ReadWriteByte(u8 TxData)
{
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
	
		SPI_I2S_SendData(SPI2, TxData); //ͨ������SPIx����һ��byte  ����
		
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte  
		return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����	
}




