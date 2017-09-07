#ifndef _SYSCALL_H_

#define _SYSCALL_H_

#ifndef _TYPE_H_
#include "../type.h"
#endif

void set_syscall_env();

//系统调用号
#define SYS_PRINTF 0
#define SYS_DELAY 1
#define SYS_GET_TICKS 2
#define SYS_KEYBOARD_READ 3
#define SYS_COLORFUL_PUTS 4
#define SYS_PUTCHAR 5
#define SYS_EXIT_PROCESS 6
#define SYS_CREATE_PROCESS 7
#define SYS_CLEAN_SCREEN 8
#define SYS_REBOOT 9
#define SYS_CONVERT_POINTER 10
#define SYS_GET_PID 11
#define SYS_GET_PRO_STATUS 12
#define SYS_ACPI_OFF 13
#define SYS_GET_TIME 14
#define SYS_READ_FLOPPY 15
#define SYS_SUPER_MODE 16
#endif
