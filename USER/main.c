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

//PC��λ���뵥Ƭ��ͨ�ŵ�Э�鲿��
u8 t_frame[32] = {0};			//��Ƭ���ɷ��͵����֡��
u8 t_head[] = {0xFF, 0xAA};		//��Ƭ������PC��֡ͷ
u8 t_tail[] = {0xFF, 0x55};		//��Ƭ������PC��֡β
u8 r_frame[32] = {0};			
u8 r_head[] = {0xFF,0x55};		//PC������λ���Ĺ̶�֡ͷ
u8 r_tail[] = {0xFF,0xAA};		//PC������λ���Ĺ̶�֡β

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

//������
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();

	delay_init(168);  	//ʱ�ӳ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ�������
	uart_init(115200);  //���ڳ�ʼ��
//	uart_init(1382400);
	LED_Init();         //LED��ʼ��	
	exit0_init();
	KEY_Init();
	my_mem_init(SRAMIN);//��ʼ���ڲ�RAM
	CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_Normal);//CAN��ʼ������ģʽ,������500Kbps    
	Init_74HC595();
	SW_Open(0); 		//ȷ����ʼʱ74HC595���Ƶ�32�����������Ϊ0
	DAC7565_Init();
	AD7328_Init();
	SPI2_Init();
	SPI2_SetSpeed(SPI_BaudRatePrescaler_4); //����ģʽ(42/4)M SPI
	power_sw_init();
	HCF4051_Init();
	
	OSInit(&err);		    	//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();	//�����ٽ���			 
	//������ʼ����
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
	
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);      //����UCOSIII
}


void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	OS_CRITICAL_ENTER();
	//����LED3����
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
					
	
	//USART2���մ�������
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
					
	//��������������Ӧ����
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
					
	//����SPI2��������
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
	//����UartSend����
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
									
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSTaskDel((OS_TCB *)0, &err);
}

//LED3��˸����
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


//USART2���մ�������
void usart2rec_task(void *p_arg)
{
	OS_ERR err;
	u16 checksum = 0; 	//У���
	u16 r_checksum = 0; 	//����֡�а�����У���
	u16 t_checksum = 0;
//	u32 OverTimeCnt = 0;	
	u8 i;
	u16 t,len;
//	u16 t_len; 		//��Ƭ������PC��λ��������֡����
	u16 r_len;		//����λ�����յ�������֡�ĳ���
	u8 type_h,type_l;
	u16 type;
	u8 select_h,select_l;
	u16 select;
	u8 status3,status2,status1,status0;		//status���ĸ��ֽ�
	u32 status = 0;						//����״̬
	u16 ad;								//AD
	u8 DA_Channel,DA_Vol;				//DA
	u8 power_sel, power_state;			//��Դ����ģ��
	u16 CANSendID;						//CAN���ͱ����������
	u8 CANSendDLC,CANErrCycle;
	u8 ret;
	u8 CANSendBuff[8] = {0};
	u16 CANRecvID;						//CAN��ȡ�����������
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
//				printf("��ȷ�������ʽ\r\n");
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
//					printf("У�����ȷ\r\n");	
					type_h = r_frame[3];
					type_l = r_frame[4];
					type = (type_h << 8)| type_l; 
					switch(type)
					{
						case 0xF001:	//CAN��������
							//������Ӧ
							CANSendID = r_frame[5] << 8 | r_frame[6];
							CANSendDLC = r_frame[7];
							CANErrCycle = r_frame[8];
							for(i = 0; i < CANSendDLC; ++i){
								CANSendBuff[i] = r_frame[9+i];
							}
							ret = CAN1_Send_Msg(CANSendBuff,CANSendDLC,CANSendID); //ret��0��ʾ�д�
							if(CANErrCycle){	//�����˴����ط�������ʱ�򲻶��ط�
								while(ret){
									ret = CAN1_Send_Msg(CANSendBuff,CANSendDLC,CANSendID);
								}	
							}
							//�ش�����
							
							break;
						case 0xF007:	//CAN���Ļ�ȡ����
							//������Ӧ
							CANRecvID = r_frame[5] << 8 | r_frame[6];	
							
							while_cnt = 0;
							while(1){
								if((RxMessage.StdId == CANRecvID) && (RxMessage.Data[1] > 0)){
									break;
								}
								else{
									while_cnt++;	//�ȴ���Ҫ�ı��ģ���ʱ������while(1)����ֹ�������
									if(while_cnt > 0xFFFFFF){
										IsReportReceiveFail	= 1;
										break;
									}	
								}		
							}
//							
							if(IsReportReceiveFail){
								IsReportReceiveFail = 0;
								break;	//������case,���ش�����		
							}
							
							CANRecvDLC = RxMessage.DLC;
							for(i=0;i<CANRecvDLC;i++){
								CANRecvBuff[i] = RxMessage.Data[i];
							}
							//�ش�����
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
						case 0xF002:	//AD����
						//������Ӧ
							select_h = r_frame[5];
							select_l = r_frame[6];
							select = (select_h << 8) | select_l;
							ad = AD7328_Sample(select);
							
						//��ʼ���ո�ʽ�ش�����
							t_frame[0] = t_head[0];
							t_frame[1] = t_head[1];
							t_frame[2] = 0x09;
							t_frame[3] = 0xF0;
							t_frame[4] = 0x02;
							t_frame[5] = select_h;
							t_frame[6] = select_l;
							t_frame[7] = (u8)(ad>>8);	//adֵ����λ
							t_frame[8] = (u8)(ad&0xFF);
							t_checksum = 0;		//������whileѭ���У�t_checksum�����Զ����㣬
												//����ϴε��ۼӺ�����
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
						case 0xF003:		//DA���
							//������Ӧ
							DA_Channel = r_frame[5];
							DA_Vol = r_frame[6] / 10.0;
							DAC7565_Output(DA_Channel,DA_Vol);
							//�ش�����
						
							break;	
						case 0xF004:		//����ģ��
							//������Ӧ
							status3 = r_frame[5];
							status2 = r_frame[6];
							status1 = r_frame[7];
							status0 = r_frame[8];
							status =  status3<<24 | status2<<16 | status1<< 8 | status0;
							SW_Open(status);	//����status�򿪺͹ر���Ӧ�Ŀ���
							status = 0;
							//�ش�����
						
							break;
						case 0xF005:		//��Դ����ģ��
							//������Ӧ
							power_sel = r_frame[5];
							power_state = r_frame[6];
							power_manager(power_sel,power_state);
							//�ش�����
						
							break;
						case 0xF006:		//��λ���汾��ѯ����
							//������Ӧ
						
							//�ش�����
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
						case 0xF0FE:		//Ҫ����λ���ش�����
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


//�û�����������Ӧ����������
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
	
//		printf("\r\n���͵�����Ϊ:\r\n");
//		for(i=0;i<8;i++)
//		{
//			canbuf[i]=cnt+i;//��䷢�ͻ�����
//			printf("%d ",canbuf[i]);
// 		}
//		printf("\r\n");
//		res=CAN1_Send_Msg(canbuf,8,0x12);//����8���ֽ� 
//		if(res)printf("����ʧ�ܦ\r\n");		//��ʾ����ʧ��
//		else printf("���ͳɹ�\r\n");	 	//��ʾ���ͳɹ�	
//		
	
		id = RxMessage.StdId;

		if(id == 0x513)//���յ�������
		{			
 			printf("���յ�������Ϊ:\r\n");
			for(i=0;i<8;i++)
			{									    
				printf("%d ", RxMessage.Data[i]);
 			}
			printf("\r\n");
		}
	}
#endif
}

//SPI2��������
void spi2_task(void *p_arg)
{
	OS_ERR err;
	u8 i;
	while(1){
		
		OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,0,&err);
		//SPI1_ReadWriteByte(0xaa);	//SPI2���߶�д�����ֽ�
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

//UartSend ��������100ms
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
