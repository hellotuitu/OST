#ifndef _GDT_H_

#define _GDT_H_

#include "protect.h"
#ifndef _TYPE_H_
#include "../type.h"
#endif

#define GDT_BASE 0x00060000
#define GDT_LENGTH 132 // 128 为64个进程 每个进程两项 2项系统使用 2项为空描述符

#define selector_flat_c 8
#define selector_flat_d 8*2

void init_descriptor(Descriptor* p_desc, unsigned int base_addr, unsigned int limit, unsigned short attribute);
void set_desc_in_gdt(Descriptor* p_desc, int gdt_index);
void set_gdt();

#endif
