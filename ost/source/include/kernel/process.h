#ifndef _PROCESS_H_

#define _PROCESS_H_

#include "../init/protect.h"
#include "../stdlib/o_stdlib.h"
#include "../stdlib/o_stdio.h"
#include "../ostlib/ostio.h"
#include "../ostlib/ostlib.h"
#include "../stdlib/o_string.h"

#include "../init/gdt.h"
#include "./pros.h"

#ifndef _TYPE_H_
#include "../type.h"
#endif


#define PROCESS_HEAD_BASE 0x0080000
#define PROCESS_BODY_BASE 0x0300000
#define PROCESS_RESERVED_MEMORRY 1024*2
#define PROCESS_HEAD_SIZE 1024
#define PROCESS_BODY_SIZE 1024*64

#define PROCESS_GLOBAL_TSS 0x50000

#define PROCESS_NUM 64
#define LDT_SIZE 2

//进程状态 PS = PROCESS STATUS
#define PS_NULL 0 // 空进程
#define PS_READY 1 // 准备就绪
#define PS_DIED 2 // 死亡进程
#define PS_RUNNING 3// 运行中的进程

typedef struct s_stackframe {
	t_32	gs;
	t_32	fs;
	t_32	es;
	t_32	ds;
	t_32	edi;
	t_32	esi;
	t_32	ebp;
	t_32	ebx;
	t_32	edx;
	t_32	ecx;
	t_32	eax;
	t_32	retaddr;
	t_32	eip;
	t_32	cs;
	t_32	eflags;
	t_32	esp;
	t_32	ss;
}Stack_Frame;

typedef struct s_proc {
	int ticks;   // 执行的ticks
	int	priority; // 优先级
	t_32 pid; // 进程ID
	char name[16]; // 进程名
  int status;
	int				nr_tty;
}Process;

// void init_process_body(void* dest, int length, int pro_index);
// void init_process_header(int pro_index);

extern Process process_info_table[64];

bool create_process(char* name, void* memory, int length, int priority);
bool exit_process();

void start_process();
void make_process_to_super_mode();

#endif
