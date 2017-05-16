#ifndef _EXIT_H
#define _EXIT_H

#include "sys.h"

#ifdef SYSTEM_SUPPORT_OS
#include "includes.h"
#endif

extern OS_TCB KEYTask_TCB;

void exit0_init(void);

#endif
