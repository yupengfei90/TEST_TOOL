#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "exit.h"
#include "can.h"
#include "spi.h"
#include "74hc595.h"
#include "DAC7565.h"
#include "AD7328.h"
#include "HCF4051.h"
#include "power_sw.h"
#include "malloc.h"
#include "includes.h"

//ALIENTEK 探索者STM32F407开发板 UCOSIII实验
//例13-1 UCOSIII 同时等待多个内核对象

//UCOSIII中以下优先级用户程序不能使用，ALIENTEK
//将这些优先级分配给了UCOSIII的5个系统内部任务
//优先级0：中断服务服务管理任务 OS_IntQTask()
//优先级1：时钟节拍任务 OS_TickTask()
//优先级2：定时任务 OS_TmrTask()
//优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
//优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK


//PC上位机与单片机通信的协议部分
u8 t_frame[32] = {0};			//单片机可发送的最大帧长
u8 t_head[] = {0xFF, 0xAA};		//单片机发往PC的帧头
u8 t_tail[] = {0xFF, 0x55};		//单片机发往PC的帧尾
u8 r_frame[32] = {0};			
u8 r_head[] = {0xFF,0x55};		//PC发往下位机的固定帧头
u8 r_tail[] = {0xFF,0xAA};		//PC发往下位机的固定帧尾

//Start task 
#define START_TASK_PRIO 3
#define START_STK_SIZE 128
OS_TCB StartTask_TCB;
CPU_STK StartTask_STK[START_STK_SIZE];
void start_task(void *p_arg);

//LED3 task
#define LED3_TASK_PRIO 4
#define LED3_STK_SIZE 128
OS_TCB LED3Task_TCB;
CPU_STK LED3Task_STK[LED3_STK_SIZE];
void led3_task(void *p_arg);

//USART2REC task
#define USART2REC_TASK_PRIO 5
#define USART2REC_STK_SIZE 128
OS_TCB 	USART2RECTask_TCB;
CPU_STK USART2RECTask_STK[USART2REC_STK_SIZE];
void usart2rec_task(void *p_arg);

//key down response
#define KEY_TASK_PRIO 6
#define KEY_STK_SIZE 128
OS_TCB KEYTask_TCB;
CPU_STK KEYTask_STK[LED3_STK_SIZE];
void key_task(void *p_arg);

//SPI2 test task
#define SPI2_TASK_PRIO 7
#define SPI2_STK_SIZE 128
OS_TCB SPI2Task_TCB;
CPU_STK SPI2Task_STK[SPI2_STK_SIZE];
void spi2_task(void *p_arg);

//主函数
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();

	delay_init(168);  	//时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组配置
	uart_init(115200);  //串口初始化
//	uart_init(2100000);
	LED_Init();         //LED初始化	
	exit0_init();
	KEY_Init();
	my_mem_init(SRAMIN);//初始化内部RAM
	CAN2_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);//CAN初始化环回模式,波特率500Kbps 	
	Init_74HC595();
	SW_Open(0); 		//确保初始时74HC595控制的32个开关输出都为0
	DAC7565_Init();
	AD7328_Init();
	SPI2_Init();
	SPI2_SetSpeed(SPI_BaudRatePrescaler_4); //高速模式(42/4)M SPI
	power_sw_init();
	HCF4051_Init();
	KL15_EN = 1;
	delay_ms(100);		//延时等待各项初始值稳定

#if 0
	while(1){
		u16 ad;
		if(KEY_User){
//			AD7328_ChannelSelect(3);
//			HCF4051_ChannelSelect(0);
			
			AD7328_Sample(Power_Vol_AD);
//			delay_ms(10);
//			AD7328_ChannelSelect(3);
//			ad = AD7328_ReadAD();
//			printf("ad = 0x%X\r\n",ad);
			delay_ms(1000);
		}
	}
#endif
	
	OSInit(&err);		    	//初始化UCOSIII
	OS_CRITICAL_ENTER();	//进入临界区			 
	//创建开始任务
	OSTaskCreate (  (OS_TCB        *)&StartTask_TCB,
					(CPU_CHAR      *)"start task",
					(OS_TASK_PTR    )start_task,
					(void          *)0,
					(OS_PRIO        )START_TASK_PRIO,
					(CPU_STK       *)StartTask_STK,
					(CPU_STK_SIZE   )START_STK_SIZE/10,
					(CPU_STK_SIZE   )START_STK_SIZE,
					(OS_MSG_QTY     )0,
					(OS_TICK        )0,
					(void          *)0,
					(OS_OPT         )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
					(OS_ERR        *)&err);
	
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);      //开启UCOSIII
}


void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	OS_CRITICAL_ENTER();
	//创建LED3任务
	OSTaskCreate (  (OS_TCB        *)&LED3Task_TCB,
					(CPU_CHAR      *)"led3 task",
					(OS_TASK_PTR    )led3_task,
					(void          *)0,
					(OS_PRIO        )LED3_TASK_PRIO,
					(CPU_STK       *)LED3Task_STK,
					(CPU_STK_SIZE   )LED3_STK_SIZE/10,
					(CPU_STK_SIZE   )LED3_STK_SIZE,
					(OS_MSG_QTY     )0,
					(OS_TICK        )0,
					(void          *)0,
					(OS_OPT         )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
					(OS_ERR        *)&err);
					
	
	//USART2接收处理任务
	OSTaskCreate (  (OS_TCB        *)&USART2RECTask_TCB,
					(CPU_CHAR      *)"usart2rec task",
					(OS_TASK_PTR    )usart2rec_task,
					(void          *)0,
					(OS_PRIO        )USART2REC_TASK_PRIO,
					(CPU_STK       *)USART2RECTask_STK,
					(CPU_STK_SIZE   )USART2REC_STK_SIZE/10,
					(CPU_STK_SIZE   )USART2REC_STK_SIZE,
					(OS_MSG_QTY     )0,
					(OS_TICK        )0,
					(void          *)0,
					(OS_OPT         )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
					(OS_ERR        *)&err);
					
	//创建按键按下响应任务
	OSTaskCreate (  (OS_TCB        *)&KEYTask_TCB,
					(CPU_CHAR      *)"key task",
					(OS_TASK_PTR    )key_task,
					(void          *)0,
					(OS_PRIO        )KEY_TASK_PRIO,
					(CPU_STK       *)KEYTask_STK,
					(CPU_STK_SIZE   )KEY_STK_SIZE/10,
					(CPU_STK_SIZE   )KEY_STK_SIZE,
					(OS_MSG_QTY     )0,
					(OS_TICK        )0,
					(void          *)0,
					(OS_OPT         )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
					(OS_ERR        *)&err);
					
	//创建SPI2测试任务
	OSTaskCreate (  (OS_TCB        *)&SPI2Task_TCB,
					(CPU_CHAR      *)"spi2 task",
					(OS_TASK_PTR    )spi2_task,
					(void          *)0,
					(OS_PRIO        )SPI2_TASK_PRIO,
					(CPU_STK       *)SPI2Task_STK,
					(CPU_STK_SIZE   )SPI2_STK_SIZE/10,
					(CPU_STK_SIZE   )SPI2_STK_SIZE,
					(OS_MSG_QTY     )0,
					(OS_TICK        )0,
					(void          *)0,
					(OS_OPT         )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
					(OS_ERR        *)&err);
					
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSTaskDel((OS_TCB *)0, &err);
}

//LED3闪烁任务
void led3_task(void *p_arg)
{
	OS_ERR err;
	
	while(1)
	{
		LED3 = ~LED3;

//		printf("1234567\n");
		
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}


void SendWrongFrame(void)
{
	u8 i;
	
	t_frame[0] = t_head[0];
	t_frame[1] = t_head[1];
	t_frame[2] = 0x03;
	t_frame[3] = 0xF0;
	t_frame[4] = 0xFF;
	t_frame[5] = t_tail[0];
	t_frame[6] = t_tail[1];
	for(i=0;i<7;++i)
		MyUSART_SendData(USART2,t_frame[i]);
}
//USART2接收处理任务
void usart2rec_task(void *p_arg)
{
	OS_ERR err;
	u16 checksum = 0; 	//校验和
	u16 t_checksum; 	//命令帧中包含的校验和
	u8 i;
	u16 t,len;
	u16 t_len; 		//PC发送的命令帧中的LEN字节
	u8 type_h,type_l;
	u16 type;
	u8 select_h,select_l;
	u16 select;
	u8 status3,status2,status1,status0;		//status的四个字节
	u32 status = 0;						//开关状态
	while(1){
		if(USART_RX_STA & 0x8000){
			printf("\r\n接收到的数据为: \r\n");
			len = (USART_RX_STA & 0x7FF);
			for(t=0;t<len;t++){
				printf("%X ",USART_RX_BUF[t]);
			}
			printf("\r\n");
			
			t_len = USART_RX_BUF[2];
			
			if (USART_RX_BUF[0] == r_head[0] && USART_RX_BUF[1] == r_head[1] &&  \
				
				USART_RX_BUF[2+t_len] == r_tail[0] && USART_RX_BUF[3+t_len] == r_tail[1])
			{
				printf("正确的命令格式\r\n");
				for(i=2;i<t_len;++i)
				{	
					checksum += USART_RX_BUF[i];
				}
				t_checksum = (USART_RX_BUF[t_len] << 8) | USART_RX_BUF[t_len+1];
				if(checksum != t_checksum){	
					printf("校验和错误，请重发\r\n");
					printf("checksum = %d,t_checksum = %d\r\n",checksum,t_checksum);
					checksum = 0;
				//	SendWrongFrame();
				} 
				else{
					printf("校验和正确\r\n");	
					checksum = 0;
					type_h = USART_RX_BUF[3];
					type_l = USART_RX_BUF[4];
					type = (type_h << 8)| type_l; 
					switch(type)
					{
						case 0xF001:
							break;
						case 0xF002:	//AD采样
				
							select_h = USART_RX_BUF[5];
							select_l = USART_RX_BUF[6];
							select = (select_h << 8) | select_l;
							AD7328_Sample(select);
					//		printf("Power_Vol_AD\r\n");
							break;					
						case 0xF003:
							break;	
						case 0xF004:	//开关模块
							status3 = USART_RX_BUF[5];
							status2 = USART_RX_BUF[6];
							status1 = USART_RX_BUF[7];
							status0 = USART_RX_BUF[8];
							status =  status3<<24 | status2<<16 | status1<< 8 | status0;
							SW_Open(status);	//根据status打开和关闭相应的开关
							status = 0;
							break;
						case 0xF005:
							break;
						case 0xF006:
							break;	
						default:
							break;
					}
				}
			}
			USART_RX_STA = 0;
		}		
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}


//用户按键按下响应任务处理任务
void key_task(void *p_arg)
{
	OS_ERR err;
	u8 i=0,res=0;
	u8 cnt=0;
	u8 canbuf[8];
	
	while(1){
		OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,0,&err);
		printf("\r\n发送的数据为:\r\n");
		for(i=0;i<8;i++)
		{
			canbuf[i]=cnt+i;//填充发送缓冲区
			printf("%d ",canbuf[i]);
 		}
		printf("\r\n");
		
		res=CAN2_Send_Msg(canbuf,8);//发送8个字节 
		if(res)printf("发送失败r\n");		//提示发送失败
		else printf("发送成功\r\n");	 		//提示发送成功	
		
		res=CAN2_Receive_Msg(canbuf);
		if(res)//接收到有数据
		{			
 			printf("接收到的数据为:\r\n");
			for(i=0;i<res;i++)
			{									    
				printf("%d ", canbuf[i]);
 			}
			printf("\r\n");
		}
		
//		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}

//SPI2测试任务
void spi2_task(void *p_arg)
{
	OS_ERR err;
//	u8 i;
	while(1){
		
		OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,0,&err);
		//SPI1_ReadWriteByte(0xaa);	//SPI2总线读写两个字节
		//SPI2_ReadWriteByte(0x55);
//		for(i=0;i<4;i++)
//			DAC7565_Output(i,i+1);	
//		range_register_set();
		
//		SPI_ReadTest();
//		AD7328_ChannelRead(0);
		
		AD7328_Sample(SIG_Sensor_AD0);
		printf(" SIG_Sensor_AD0\r\n");
		AD7328_Sample(SIG_Sensor_AD1);
		printf(" SIG_Sensor_AD1\r\n");
		AD7328_Sample(SIG_Sensor_AD2);
		printf(" SIG_Sensor_AD2\r\n");
		AD7328_Sample(SIG_Sensor_AD3);
		printf(" SIG_Sensor_AD3\r\n");
		AD7328_Sample(SIG_Sensor_AD4);
		printf(" SIG_Sensor_AD4\r\n");

//		AD7328_Sample(SIG_Trig_Low_AD8);
//		printf(" SIG_Trig_Low_AD8\r\n");

		printf("\r\n");

//		DAC7565_Output(0,3.6);
//		SPI2_ReadWriteByte(0x55);
		
		//OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}


