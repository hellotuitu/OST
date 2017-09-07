#include "../include/headers.h"

// ---------------------------------------------

// 设置idt

// 该模块导出：
//   set_idt set_realmode_8259a

// 该模块导入：
//   set_irq set_syscall_env

// -------------------------------------------

static void* idt_base = (void *)IDT_BASE;
static unsigned short idt_size = IDT_SIZE;
static Gate* idt = (Gate*)IDT_BASE;

// unsigned char realmode_8259a_imr = 0;
// 暂时未使用 注释之

void init_8259A(){
	//保存实模式中断屏蔽寄存器
	// realmode_8259a_imr = inbyte(INT_M_CTLMASK);

	outbyte_d(INT_M_CTL,	0x11);
	outbyte_d(INT_S_CTL,	0x11);

	outbyte_d(INT_M_CTLMASK,	INT_VECTOR_IRQ0);
	outbyte_d(INT_S_CTLMASK,	INT_VECTOR_IRQ8);

	outbyte_d(INT_M_CTLMASK,	0x4);
	outbyte_d(INT_S_CTLMASK,	0x2);

	outbyte_d(INT_M_CTLMASK,	0x1);
	outbyte_d(INT_S_CTLMASK,	0x1);

	outbyte_d(INT_M_CTLMASK,	0xFF);
	outbyte_d(INT_S_CTLMASK,	0xFF);
}

void set_realmode_8259a(){
	outbyte_d(0x20, 0x11);
	outbyte_d(0xa0, 0x11);

	// 设置主 8259A 偏移 irq0—irq7 ->  0x8 - 0xf
	outbyte_d(0x21, 0x8);
	// 设置从 8259A 偏移 irq8 - irq15 -> 0x70 - 0x77
	outbyte_d(0xa1, 0x70);

	outbyte_d(0x21, 0x4);
	outbyte_d(0xa1, 0x2);

	outbyte_d(0x21, 0x3);
	outbyte_d(0xa1, 0x1);

	outbyte_d(0x21, 0);
	outbyte_d(0xa1, 0);

}

static void init_idt_desc(unsigned char vector, unsigned char desc_type, t_pf_int_handler handler, unsigned char privilege){
	Gate *	p_gate	= &idt[vector];
	t_32	base	= (t_32)handler;
	p_gate->offset_low	= base & 0xFFFF;
	p_gate->selector	= selector_flat_c;
	p_gate->dcount		= 0;
	p_gate->attr		= desc_type | (privilege << 5);
	p_gate->offset_high	= (base >> 16) & 0xFFFF;
}

static void init_idt(){
  memset(idt_base, 0, idt_size * 8);
}

static void update_idtr(){
	unsigned short idt_limit = idt_size * 8 - 1;
  memcpy(idt_base + idt_size * 8, &idt_limit, 2);
  memcpy(idt_base + idt_size * 8 + 2, &idt_base, 4);

  asm volatile(
    "mov esi, %[idt_ptr];"
    "lidt	[esi]"
    :
    : [idt_ptr]"r"(idt_base + idt_size * 8)
    : "esi"
  );
}

void set_idt(){
  init_8259A();

  init_idt();

	init_idt_desc(INT_VECTOR_DIVIDE,	DA_386IGate, divide_error,		PRIVILEGE_USER);
	init_idt_desc(INT_VECTOR_DEBUG,		DA_386IGate, single_step_exception,	PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_NMI,		DA_386IGate, nmi,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_BREAKPOINT,	DA_386IGate, breakpoint_exception,	PRIVILEGE_USER);
	init_idt_desc(INT_VECTOR_OVERFLOW,	DA_386IGate, overflow,			PRIVILEGE_USER);
	init_idt_desc(INT_VECTOR_BOUNDS,	DA_386IGate, bounds_check,		PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_INVAL_OP,	DA_386IGate, inval_opcode,		PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_COPROC_NOT,	DA_386IGate, copr_not_available,	PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_DOUBLE_FAULT,	DA_386IGate, double_fault,		PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_COPROC_SEG,	DA_386IGate, copr_seg_overrun,		PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_INVAL_TSS,	DA_386IGate, inval_tss,			PRIVILEGE_USER);
	init_idt_desc(INT_VECTOR_SEG_NOT,	DA_386IGate, segment_not_present,	PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_STACK_FAULT,	DA_386IGate, stack_exception,		PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_PROTECTION,	DA_386IGate, general_protection,	PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_PAGE_FAULT,	DA_386IGate, page_fault,		PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_COPROC_ERR,	DA_386IGate, copr_error,		PRIVILEGE_KRNL);


	init_idt_desc(INT_VECTOR_IRQ0 + 0,	DA_386IGate, hwint00,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 1,	DA_386IGate, hwint01,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 2,	DA_386IGate, hwint02,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 3,	DA_386IGate, hwint03,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 4,	DA_386IGate, hwint04,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 5,	DA_386IGate, hwint05,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 6,	DA_386IGate, hwint06,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 7,	DA_386IGate, hwint07,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ8 + 0,	DA_386IGate, hwint08,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ8 + 1,	DA_386IGate, hwint09,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ8 + 2,	DA_386IGate, hwint10,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ8 + 3,	DA_386IGate, hwint11,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ8 + 4,	DA_386IGate, hwint12,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ8 + 5,	DA_386IGate, hwint13,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ8 + 6,	DA_386IGate, hwint14,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ8 + 7,	DA_386IGate, hwint15,			PRIVILEGE_KRNL);

	init_idt_desc(0x79,	DA_386IGate, debug_int,			PRIVILEGE_USER); //用于bochs下断点
	init_idt_desc(0x78,	DA_386IGate, sys_call,			PRIVILEGE_USER); //用于系统调用

  set_irq();
  set_syscall_env();
  set_clock_frequency();
	
  update_idtr();
}
