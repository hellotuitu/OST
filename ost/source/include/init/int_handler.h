#ifndef _INT_HANDLER_H_

#define _INT_HANDLER_H_

#include "protect.h"
#ifndef _TYPE_H_
#include "../type.h"
#endif

void exception_handler(int vec_no, int err_code, int eip, int cs, int eflags);

void	divide_error();
void	single_step_exception();
void	nmi();
void	breakpoint_exception();
void	overflow();
void	bounds_check();
void	inval_opcode();
void	copr_not_available();
void	double_fault();
void	copr_seg_overrun();
void	inval_tss();
void	segment_not_present();
void	stack_exception();
void	general_protection();
void	page_fault();
void	copr_error();
void	hwint00();
void	hwint01();
void	hwint02();
void	hwint03();
void	hwint04();
void	hwint05();
void	hwint06();
void	hwint07();
void	hwint08();
void	hwint09();
void	hwint10();
void	hwint11();
void	hwint12();
void	hwint13();
void	hwint14();
void	hwint15();

//硬件中断号的定义
#define IRQ_CLOCK 0x20
#define IRQ_KEYBOARD IRQ_CLOCK + 1

// irq处理
void clock_handler(); // 时钟中断处理程序
void fake_clock();

void keyboard_handler(); //键盘中断处理

//设置时钟中断频率
void set_clock_frequency();

// 软中断
void debug_int(); // 用于bochs下断点
void sys_call();

//irq入口
void set_irq();

// 工具函数
void enable_irq(int) ;
void disable_irq(int);

#endif
