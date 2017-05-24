	空调控制器测试工装程序：该程序用于测试工装，用于检测空调空调控制器是否合格。
	为了解决多任务管理的复杂性，增强程序的适应性可移植性。该程序内嵌了UCOSIII操作系统。UCOS是一个开放源代码的
操作系统以其代码的规范简洁，实时性高而广为嵌入式设备所采用。


/****************************************************************
	AD7328数据格式
******************************************************************/
typedef union{
	u16 data;
	struct {
		u16 lsb:1;
		u16 weak:1;
		u16 seq2:1;
		u16 seq1:1;
		u16 ref:1;
		u16 coding:1;
		u16 pm0:1;
		u16 pm1:1;
		u16 mode0:1;
		u16 mode1:1;
		u16 addr0:1;
		u16 addr1:1;
		u16 addr2:1;
		u16 register_select2:1;
		u16 register_select1:1;
		u16 write:1;
	}bits;
}AD_Cycle;


//初始化
	AD_Cycle AD1 = {0};

//对需要改变的位重新赋值
//	AD1.bits.lsb = 0; 	//固定为0
//	AD1.bits.weak = 0;	//一个周期传输完成返回three-state状态
//	AD1.bits.seq2 = 0;	//不使用sequcer
//	AD1.bits.seq1 = 0;
	AD1.bits.ref = 1;	//选择内部reference
//	AD1.bits.coding = 0;//原始二进制数据输出
//	AD1.bits.pm0 = 0;	//电源配置为正常使用模式
//	AD1.bits.pm1 = 0;
//	AD1.bits.mode0 = 0;	//单通道输入模式
//	AD1.bits.mode1 = 0;
//	AD1.bits.addr0 = 0;	//选择通道0
//	AD1.bits.addr1 = 0;
//	AD1.bits.addr2 = 0;
//	AD1.bits.register_select1 = 0; //选择控制寄存器
//	AD1.bits.register_select2 = 0;
	AD1.bits.write = 1;	//写入有效
	
	printf("AD1.data = 0x%x\r\n", AD1.data);