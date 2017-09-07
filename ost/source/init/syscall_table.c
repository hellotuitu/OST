#include "../include/headers.h"

// ---------------------------------------------

// 系统调用总入口

// 该模块导出：
//   system 函数指针
//   pro_delay 函数指针
//   syscall_table
//   set_syscall_env

// 该模块导入：
//   各API的地址


// -------------------------------------------

void* syscall_table[100];

static void put_syscall_handler(int syscall_num, void* handler){
	syscall_table[syscall_num] = handler;
}

void set_syscall_env(){
  put_syscall_handler(SYS_PRINTF, &printf);
  put_syscall_handler(SYS_DELAY, &delay);
  put_syscall_handler(SYS_GET_TICKS, &get_ticks);
  put_syscall_handler(SYS_KEYBOARD_READ, &keyboard_read);
  put_syscall_handler(SYS_COLORFUL_PUTS, &ost_colorful_puts);
  put_syscall_handler(SYS_PUTCHAR, &ost_putchar);
  put_syscall_handler(SYS_EXIT_PROCESS, &exit_process);
  put_syscall_handler(SYS_CREATE_PROCESS, &create_process);
  put_syscall_handler(SYS_CLEAN_SCREEN, &clean_screen);
  put_syscall_handler(SYS_REBOOT, &do_reboot);
  put_syscall_handler(SYS_GET_PID, &get_pid);
  put_syscall_handler(SYS_CONVERT_POINTER, &convert_pointer_to_global);
  put_syscall_handler(SYS_GET_PRO_STATUS, &get_pro_status);
  put_syscall_handler(SYS_ACPI_OFF, &acpiPowerOff);
  put_syscall_handler(SYS_GET_TIME, &get_time);
  put_syscall_handler(SYS_READ_FLOPPY, &floppy_api);
  put_syscall_handler(SYS_SUPER_MODE, &make_process_to_super_mode);
}
