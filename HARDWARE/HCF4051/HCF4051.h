#ifndef _HCF4051_H
#define _HCF4051_H
#include "sys.h"

#define HCF4051_A PDout(11)
#define HCF4051_B PDout(12)
#define HCF4051_C PDout(10)

/*=============================================
	Global Function
 ==============================================*/
 void HCF4051_Init(void);
 void HCF4051_ChannelSelect(u8 channel);
#endif
