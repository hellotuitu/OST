#include "../include/stdlib/o_stdio.h"
#include "../include/ostlib/ostio.h"
#include "../include/ostlib/ostlib.h"
#include "../include/init/int_handler.h"

// ---------------------------------------------

// 异常处理

// 该模块导出：
//   exception_handler

// 该模块导入：
//   无


// -------------------------------------------

//对异常的处理
//暂时只是显示异常 然后处理器休眠
void exception_handler(int vec_no, int err_code, int eip, int cs, int eflags)
{
	char err_description[][64] = {	"#DE Divide Error",
					"#DB RESERVED",
					"—  NMI Interrupt",
					"#BP Breakpoint",
					"#OF Overflow",
					"#BR BOUND Range Exceeded",
					"#UD Invalid Opcode (Undefined Opcode)",
					"#NM Device Not Available (No Math Coprocessor)",
					"#DF Double Fault",
					"    Coprocessor Segment Overrun (reserved)",
					"#TS Invalid TSS",
					"#NP Segment Not Present",
					"#SS Stack-Segment Fault",
					"#GP General Protection",
					"#PF Page Fault",
					"—  (Intel reserved. Do not use.)",
					"#MF x87 FPU Floating-Point Error (Math Fault)",
					"#AC Alignment Check",
					"#MC Machine Check",
					"#XF SIMD Floating-Point Exception"
				};

  printf("Exception happened!\n");
  printf("Descpption:%s\n", err_description[vec_no]);
  printf("Vec_Num:%x\n", vec_no);
  printf("Eflags:%x\n", eflags);
  printf("Cs:%x\n", cs);
  printf("Eip:%p\n", eip);

	if(err_code != 0xFFFFFFFF){
		printf("Error_Code:%x\n", err_code);
	}

  halt();
}
