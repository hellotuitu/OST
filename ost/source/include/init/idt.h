#ifndef _IDT_H_

#define _IDT_H_

#include "protect.h"
#ifndef _TYPE_H_
#include "../type.h"
#endif

#define IDT_BASE 0x70000
#define IDT_SIZE 128

#define	INT_M_CTL	 0x20
#define	INT_M_CTLMASK	 0x21
#define	INT_S_CTL	 0xA0
#define	INT_S_CTLMASK	 0xA1


void set_idt();
void set_realmode_8259a();

#endif
