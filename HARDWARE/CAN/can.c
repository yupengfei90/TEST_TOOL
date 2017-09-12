#define _CAN_C
#include "can.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif

/*==========================================================================
		Global Variable
  ==========================================================================*/
CanRxMsg RxMessage;
CanRxMsg RxMessage_0x513;
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//CAN驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/7
//版本：V1.0 
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024; tq=(brp)*tpclk1
//波特率=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
//Fpclk1的时钟在初始化的时候设置为42M,如果设置CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
//则波特率为:42M/((6+7+1)*6)=500Kbps
//返回值:0,初始化OK;
//    其他,初始化失败; 
u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{

  	GPIO_InitTypeDef GPIO_InitStructure; 
	  CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
#if CAN1_RX0_INT_ENABLE 
   	NVIC_InitTypeDef  NVIC_InitStructure;
#endif
    //使能相关时钟
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能PORTA时钟	                   											 

  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	
	
    //初始化GPIO 复用引脚
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化PD0,PD1
	
	//初始化GPIO TJA1051T使能引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出模式
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化PD4
	GPIO_ResetBits(GPIOD,GPIO_Pin_4);
	
	  //引脚复用映射配置
	  GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_CAN1); //GPIOD0复用为CAN1
	  GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_CAN1); //GPIOD1复用为CAN1
	  
  	//CAN单元设置
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//非时间触发通信模式   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	//软件自动离线管理	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure.CAN_NART=ENABLE;	//禁止报文自动传送 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//报文不锁定,新的覆盖旧的  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//优先级由报文标识符决定 
  	CAN_InitStructure.CAN_Mode= mode;	 //模式设置 
  	CAN_InitStructure.CAN_SJW=tsjw;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;  //分频系数(Fdiv)为brp+1	
  	CAN_Init(CAN1, &CAN_InitStructure);   // 初始化CAN1 
    
		//配置过滤器
 	  CAN_FilterInitStructure.CAN_FilterNumber=0;	  //过滤器0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32位ID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0
  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
		
#if CAN1_RX0_INT_ENABLE
	
	  CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}   
 

#if CAN1_RX0_INT_ENABLE	//使能RX0中断
//中断服务函数			    
void CAN1_RX0_IRQHandler(void)
{
//	int i=0;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	switch(RxMessage.StdId)
	{
		case 0x513:
			RxMessage_0x513 = RxMessage;
			break;
		case 0x514:
			break;
		case 0x530:
			break;
		default:
			break;	
	}		

//	printf("\r\nDLC = %d, ",RxMessage.DLC);
//	for(i=0;i<8;i++)
//	printf("rxbuf[%d]:%d ",i,RxMessage.Data[i]);
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
}
#endif

//can发送一组数据
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//id:标识符
//返回值:0,成功;
//		 其他,失败;
u8 CAN1_Send_Msg(u8* msg,u8 len,u16 id)
{	
  u8 mbox;
  u16 i=0;
  CanTxMsg TxMessage;
  TxMessage.StdId=id;	 			// 标准标识符
  TxMessage.ExtId=0x12;	 			// 设置扩展标示符
  TxMessage.IDE=CAN_Id_Standard; 	// 标准帧
  TxMessage.RTR=CAN_RTR_DATA;	 	// 数据帧
  TxMessage.DLC=len;				// 发送的帧长
  for(i=0;i<len;i++)
  TxMessage.Data[i]=msg[i];			      
  mbox= CAN_Transmit(CAN1, &TxMessage);   
  i=0;
  while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
  if(i>=0XFFF)return 1;				//时间限制
  return 0;		

}


/************************************************************************************
函数名：CAN1_Receive_Msg
功能：获取制定ID的报文数据
参数：id,需要获取的报文的ID；data,存放获取到的报文数据的数组
返回值：当获取到指定ID的报文时，返回该报文的数据长度DLC,如不是想要的ID,返回0
*************************************************************************************/
u8 CAN1_Receive_Msg(u16 id, u8 *data)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
	i = CAN_MessagePending(CAN1,CAN_FIFO0);
    if( i==0)return 0;		//没有接收到数据,直接退出 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//读取数据	
	i = CAN_MessagePending(CAN1,CAN_FIFO0);
	if(RxMessage.StdId == id){
		for(i=0;i<RxMessage.DLC;i++)
		data[i]=RxMessage.Data[i]; 
		return RxMessage.DLC;	
	}
	else{
		return 0;
	}
}














