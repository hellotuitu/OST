#ifndef _PROS_IN_H_

#define _PROS_IN_H_

// note:
//   this head file is wrote for process
//   it means only a process should include this file
//
//   there are many macros in this to implement system calls
//   in this header file

// when writing a process function
// remember to use set_system macro to init system call environment
//
// don't use any global variables or strings in a process
//
// when pass a pointer param to system calls, use p() to change
// the pointer to global, so kernel can use the pointer rightly
//
// when finished the process, don't forget call exit() to kill the process
// or there will be a fatal error!

#define	BLACK	0x0	/* 0000 */
#define	WHITE	0x7	/* 0111 */
#define	RED	0x4	/* 0100 */
#define	GREEN	0x2	/* 0010 */
#define	BLUE	0x1	/* 0001 */
#define	FLASH	0x80	/* 1000 0000 */
#define	BRIGHT	0x08	/* 0000 1000 */
#define	make_color(x,y)	((x<<4) | y)	/* MAKE_COLOR(Background,Foreground) */

#include "../init/syscall.h"
// 系统调用以宏的形式表现
#define p(x) system(SYS_CONVERT_POINTER, x)
#define printf(...) system(SYS_PRINTF, __VA_ARGS__)
#define puts(x) system(SYS_PRINTF,x)
#define putchar(x) system(SYS_PUTCHAR,x)
#define puts_c(x, y) system(SYS_COLORFUL_PUTS, x, y)
#define execute(x, y) system(SYS_CREATE_PROCESS, x, y, 2000)
#define exit() system(SYS_EXIT_PROCESS)
#define wait(x) system(SYS_GET_PRO_STATUS, x)
#define read_k() (system(SYS_KEYBOARD_READ))
#define reboot_s() system(SYS_REBOOT)
#define clear_s() system(SYS_CLEAN_SCREEN)
#define get_ticks() system(SYS_GET_TICKS)
#define get_pid() system(SYS_GET_PID)
#define shutdown_s() system(SYS_ACPI_OFF)
#define get_time(x) system(SYS_GET_TIME, x)
#define read_floppy(x, y) system(SYS_READ_FLOPPY, x, y, 0)
#define supermode() system(SYS_SUPER_MODE)
#define sleep(x) do{ \
  int a = get_ticks(); \
  while((get_ticks() - a)  < x * 100); \
}while(0)

#define set_system int(* system)(int, ...); system = 0

typedef struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
} Time;

#endif
