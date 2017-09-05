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

//Uart periodly send task
#define UartSend_TASK_PRIO 7
#define UartSend_STK_SIZE 128
OS_TCB UartSendTask_TCB;
CPU_STK UartSendTask_STK[UartSend_STK_SIZE];
void UartSend_task(void *p_arg);

//主函数
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();

	delay_init(168);  	//时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组配置
	uart_init(115200);  //串口初始化
//	uart_init(1382400);
	LED_Init();         //LED初始化	
	exit0_init();
	KEY_Init();
	my_mem_init(SRAMIN);//初始化内部RAM
	CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_Normal);//CAN初始化环回模式,波特率500Kbps    
	Init_74HC595();
	SW_Open(0); 		//确保初始时74HC595控制的32个开关输出都为0
	DAC7565_Init();
	AD7328_Init();
	SPI2_Init();
	SPI2_SetSpeed(SPI_BaudRatePrescaler_4); //高速模式(42/4)M SPI
	power_sw_init();
	HCF4051_Init();
	
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
	//创建UartSend任务
	OSTaskCreate (  (OS_TCB        *)&UartSendTask_TCB,
					(CPU_CHAR      *)"UartSend task",
					(OS_TASK_PTR    )UartSend_task,
					(void          *)0,
					(OS_PRIO        )UartSend_TASK_PRIO,
					(CPU_STK       *)UartSendTask_STK,
					(CPU_STK_SIZE   )UartSend_STK_SIZE/10,
					(CPU_STK_SIZE   )UartSend_STK_SIZE,
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


static void SendWrongFrame(void)
{
	u8 i;
	
	t_frame[0] = t_head[0];
	t_frame[1] = t_head[1];
	t_frame[2] = 0x05;
	t_frame[3] = 0xF0;
	t_frame[4] = 0xFF;
	t_frame[5] = 0x01;
	t_frame[6] = 0xF4;
	t_frame[7] = t_tail[0];
	t_frame[8] = t_tail[1];
	for(i=0;i<9;++i)
		MyUSART_SendData(TEAM_PORT,t_frame[i]);
}
static void SendFrame(void)
{
	u8 i;
	
	for(i=0;i<32;i++){
		MyUSART_SendData(TEAM_PORT,t_frame[i]);
	}
}


//USART2接收处理任务
void usart2rec_task(void *p_arg)
{
	OS_ERR err;
	u16 checksum = 0; 	//校验和
	u16 r_checksum = 0; 	//命令帧中包含的校验和
	u16 t_checksum = 0;
//	u32 OverTimeCnt = 0;	
	u8 i;
	u16 t,len;
//	u16 t_len; 		//单片机发往PC上位机的命令帧长度
	u16 r_len;		//从上位机接收到的命令帧的长度
	u8 type_h,type_l;
	u16 type;
	u8 select_h,select_l;
	u16 select;
	u8 status3,status2,status1,status0;		//status的四个字节
	u32 status = 0;						//开关状态
	u16 ad;								//AD
	u8 DA_Channel,DA_Vol;				//DA
	u8 power_sel, power_state;			//电源管理模块
	u16 CANSendID;						//CAN发送报文命令变量
	u8 CANSendDLC,CANErrCycle;
	u8 ret;
	u8 CANSendBuff[8] = {0};
	u16 CANRecvID;						//CAN获取报文命令变量
	u8 CANRecvDLC;		
	u8 CANRecvBuff[8] = {0};
	u32 while_cnt;
	u8 IsReportReceiveFail = 0;

	while(1){
		OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,0,&err);

			len = (USART_RX_STA & 0x7FF);
		
			for(t=0;t<len;t++){
				r_frame[t] = USART_RX_BUF[t];
			}
			
			r_len = r_frame[2];
			
			if (r_frame[0] == r_head[0] && r_frame[1] == r_head[1] &&  \
				
				r_frame[2+r_len] == r_tail[0] && r_frame[3+r_len] == r_tail[1])
			{
//				printf("正确的命令格式\r\n");
				checksum = 0;
				for(i=2;i<r_len;++i)
				{	
					checksum += r_frame[i];
				}
				r_checksum = (r_frame[r_len] << 8) | r_frame[r_len+1];
				if(checksum != r_checksum){	
					checksum = 0;
					SendWrongFrame();					
				} 
				else{
//					printf("校验和正确\r\n");	
					type_h = r_frame[3];
					type_l = r_frame[4];
					type = (type_h << 8)| type_l; 
					switch(type)
					{
						case 0xF001:	//CAN发送命令
							//命令响应
							CANSendID = r_frame[5] << 8 | r_frame[6];
							CANSendDLC = r_frame[7];
							CANErrCycle = r_frame[8];
							for(i = 0; i < CANSendDLC; ++i){
								CANSendBuff[i] = r_frame[9+i];
							}
							ret = CAN1_Send_Msg(CANSendBuff,CANSendDLC,CANSendID); //ret非0表示有错
							if(CANErrCycle){	//定义了错误重发，出错时则不断重发
								while(ret){
									ret = CAN1_Send_Msg(CANSendBuff,CANSendDLC,CANSendID);
								}	
							}
							//回传数据
							
							break;
						case 0xF007:	//CAN报文获取命令
							//命令响应
							CANRecvID = r_frame[5] << 8 | r_frame[6];	
							
							while_cnt = 0;
							while(1){
								if((RxMessage.StdId == CANRecvID) && (RxMessage.Data[1] > 0)){
									break;
								}
								else{
									while_cnt++;	//等待想要的报文，超时则跳出while(1)，防止程序假死
									if(while_cnt > 0xFFFFFF){
										IsReportReceiveFail	= 1;
										break;
									}	
								}		
							}
//							
							if(IsReportReceiveFail){
								IsReportReceiveFail = 0;
								break;	//跳出本case,不回传数据		
							}
							
							CANRecvDLC = RxMessage.DLC;
							for(i=0;i<CANRecvDLC;i++){
								CANRecvBuff[i] = RxMessage.Data[i];
							}
							//回传数据
							t_frame[0] = t_head[0];
							t_frame[1] = t_head[1];
							t_frame[2] = CANRecvDLC + 8;
							t_frame[3] = 0xF0;
							t_frame[4] = 0x07;
							t_frame[5] = CANRecvID >> 8;
							t_frame[6] = CANRecvID & 0xFF;
							t_frame[7] = CANRecvDLC;
							for(i=0;i<CANRecvDLC;++i){
								t_frame[8+i] = CANRecvBuff[i];
							}
							t_checksum = 0;
							for(i=2;i<CANRecvDLC+8;++i){
								t_checksum += t_frame[i];
							}
							t_frame[8+CANRecvDLC] = t_checksum >> 8;
							t_frame[9+CANRecvDLC] = t_checksum & 0xFF;
							t_frame[10+CANRecvDLC] = t_tail[0];
							t_frame[11+CANRecvDLC] =t_tail[1];

							SendFrame();
							break;
						case 0xF002:	//AD采样
						//命令响应
							select_h = r_frame[5];
							select_l = r_frame[6];
							select = (select_h << 8) | select_l;
							ad = AD7328_Sample(select);
							
						//开始按照格式回传数据
							t_frame[0] = t_head[0];
							t_frame[1] = t_head[1];
							t_frame[2] = 0x09;
							t_frame[3] = 0xF0;
							t_frame[4] = 0x02;
							t_frame[5] = select_h;
							t_frame[6] = select_l;
							t_frame[7] = (u8)(ad>>8);	//ad值高四位
							t_frame[8] = (u8)(ad&0xFF);
							t_checksum = 0;		//程序处于while循环中，t_checksum不会自动清零，
												//须吧上次的累加和清零
							for(i=2;i<9;++i)
							{	
								t_checksum += t_frame[i];
							}
							t_frame[9] = (u8)(t_checksum >> 8);
							t_frame[10] = (u8)(t_checksum & 0xFF);	
							t_frame[11] = t_tail[0];
							t_frame[12] = t_tail[1];
							SendFrame();
							break;					
						case 0xF003:		//DA输出
							//命令响应
							DA_Channel = r_frame[5];
							DA_Vol = r_frame[6] / 10.0;
							DAC7565_Output(DA_Channel,DA_Vol);
							//回传数据
						
							break;	
						case 0xF004:		//开关模块
							//命令响应
							status3 = r_frame[5];
							status2 = r_frame[6];
							status1 = r_frame[7];
							status0 = r_frame[8];
							status =  status3<<24 | status2<<16 | status1<< 8 | status0;
							SW_Open(status);	//根据status打开和关闭相应的开关
							status = 0;
							//回传数据
						
							break;
						case 0xF005:		//电源管理模块
							//命令响应
							power_sel = r_frame[5];
							power_state = r_frame[6];
							power_manager(power_sel,power_state);
							//回传数据
						
							break;
						case 0xF006:		//下位机版本查询命令
							//命令响应
						
							//回传数据
							t_frame[0] = t_head[0];
							t_frame[1] = t_head[1];
							t_frame[2] = 0x07;
							t_frame[3] = 0xF0;
							t_frame[4] = 0x06;
							t_frame[5] = 0xC4;
							t_frame[6] = 0x0D;
							t_frame[7] = 0x01;
							t_frame[8] = 0xCE;
							t_frame[9] = t_tail[0];
							t_frame[10] = t_tail[1];
							SendFrame();
							break;	
						case 0xF0FE:		//要求下位机重传命令
							SendFrame();
							break;
						default:
							break; 
					}
				}
			}
			USART_RX_STA = 0;
	}
}


//用户按键按下响应任务处理任务
void key_task(void *p_arg)
{
#if 0
	OS_ERR err;
	u8 i=0,res=0;
	u8 cnt=0;
	u8 canbuf[8];
	u16 id;
	while(1){
		OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,0,&err);
	
//		printf("\r\n发送的数据为:\r\n");
//		for(i=0;i<8;i++)
//		{
//			canbuf[i]=cnt+i;//填充发送缓冲区
//			printf("%d ",canbuf[i]);
// 		}
//		printf("\r\n");
//		res=CAN1_Send_Msg(canbuf,8,0x12);//发送8个字节 
//		if(res)printf("发送失败r\n");		//提示发送失败
//		else printf("发送成功\r\n");	 	//提示发送成功	
//		
	
		id = RxMessage.StdId;

		if(id == 0x513)//接收到有数据
		{			
 			printf("接收到的数据为:\r\n");
			for(i=0;i<8;i++)
			{									    
				printf("%d ", RxMessage.Data[i]);
 			}
			printf("\r\n");
		}
	}
#endif
}

//SPI2测试任务
void spi2_task(void *p_arg)
{
	OS_ERR err;
	u8 i;
	while(1){
		
		OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,0,&err);
		//SPI1_ReadWriteByte(0xaa);	//SPI2总线读写两个字节
		//SPI2_ReadWriteByte(0x55);
		for(i=0;i<4;i++)
			DAC7565_Output(i,i+1);	
//		range_register_set();
		
//		SPI_ReadTest();
//		AD7328_ChannelRead(0);
		
//		AD7328_Sample(SIG_Sensor_AD0);
//		printf(" SIG_Sensor_AD0\r\n");
//		AD7328_Sample(SIG_Sensor_AD1);
//		printf(" SIG_Sensor_AD1\r\n");
//		AD7328_Sample(SIG_Sensor_AD2);
//		printf(" SIG_Sensor_AD2\r\n");
//		AD7328_Sample(SIG_Sensor_AD3);
//		printf(" SIG_Sensor_AD3\r\n");
//		AD7328_Sample(SIG_Sensor_AD4);
//		printf(" SIG_Sensor_AD4\r\n");

//		AD7328_Sample(SIG_Trig_Low_AD8);
//		printf(" SIG_Trig_Low_AD8\r\n");

//		printf("\r\n");

//		DAC7565_Output(0,3.6);
//		SPI2_ReadWriteByte(0x55);
		
		//OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}

//UartSend 发送周期100ms
void UartSend_task(void *p_arg)
{
	OS_ERR err;
	u8 i;
	u8 data[32] = {0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51};
	while(1){
		for(i=0;i<32;i++){
			MyUSART_SendData(TEAM_PORT,data[i]);
		}
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}
