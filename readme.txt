	�յ����������Թ�װ���򣺸ó������ڲ��Թ�װ�����ڼ��յ��յ��������Ƿ�ϸ�
	Ϊ�˽�����������ĸ����ԣ���ǿ�������Ӧ�Կ���ֲ�ԡ��ó�����Ƕ��UCOSIII����ϵͳ��UCOS��һ������Դ�����
����ϵͳ�������Ĺ淶��࣬ʵʱ�Ը߶���ΪǶ��ʽ�豸�����á�


/****************************************************************
	AD7328���ݸ�ʽ
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


//��ʼ��
	AD_Cycle AD1 = {0};

//����Ҫ�ı��λ���¸�ֵ
//	AD1.bits.lsb = 0; 	//�̶�Ϊ0
//	AD1.bits.weak = 0;	//һ�����ڴ�����ɷ���three-state״̬
//	AD1.bits.seq2 = 0;	//��ʹ��sequcer
//	AD1.bits.seq1 = 0;
	AD1.bits.ref = 1;	//ѡ���ڲ�reference
//	AD1.bits.coding = 0;//ԭʼ�������������
//	AD1.bits.pm0 = 0;	//��Դ����Ϊ����ʹ��ģʽ
//	AD1.bits.pm1 = 0;
//	AD1.bits.mode0 = 0;	//��ͨ������ģʽ
//	AD1.bits.mode1 = 0;
//	AD1.bits.addr0 = 0;	//ѡ��ͨ��0
//	AD1.bits.addr1 = 0;
//	AD1.bits.addr2 = 0;
//	AD1.bits.register_select1 = 0; //ѡ����ƼĴ���
//	AD1.bits.register_select2 = 0;
	AD1.bits.write = 1;	//д����Ч
	
	printf("AD1.data = 0x%x\r\n", AD1.data);