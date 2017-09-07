#include "../../include/headers.h"

// ---------------------------------------------

// irq统一入口

// 该模块导出：
//   irq_table

// 该模块导入：
//   disable_irq enable_irq
//   各irq中断处理函数


// -------------------------------------------

// ---------------------------------------------

// 添加一个irq处理的步骤
// 1：在init/irq目录下实现该irq处理函数
// 2：在int_handler.h 中声明该函数
// 3：在set_irq()函数中调用 put_irq_handler 和 enable_irq 注册该irq处理函数

// 本文件中使用的
// disable_irq 和 enable_irq
// 在 init/int.asm 中实现

// -------------------------------------------


//irq中断处理
//该符号将在 int.asm 中被syscall使用
void floppy_handler();
void* irq_table[8];

//设置中断处理函数
 void put_irq_handler(int irq, void* handler)
{
	disable_irq(irq);
	irq_table[irq] = handler;
}

void set_irq(){
  put_irq_handler(0, &clock_handler);
  enable_irq(0); //时钟中断

	put_irq_handler(1, &keyboard_handler);
	init_keyboard();  // 该函数在init/irq/keyboard.c 中实现
	enable_irq(1); // 键盘中断

	put_irq_handler(6, &floppy_handler);
	enable_irq(6);
}
