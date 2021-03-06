#ifndef _POWER_SW_H
#define _POWER_SW_H
#include "sys.h"

/*****************************************************************
	Macro
******************************************************************/
#define KL15_EN PEout(7)
#define KL30_EN PEout(8)
#define SW_POWER PDout(6)

/*****************************************************************
	Global function
******************************************************************/
void power_sw_init(void);
void power_manager(u8 sel, u8 state);
#endif
