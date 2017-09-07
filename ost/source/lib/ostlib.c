#include "../include/headers.h"
// ---------------------------------------------

// 系统底层非IO库

// 该模块导出：
//   halt
//   do_system
//   get_ticks
//   do_reboot
//   convert_pointer_to_global
//   convert_pointer_to_local
//   get_pid
//   get_pro_status

// 该模块导入：
//   无


// -------------------------------------------

void halt(){
  ost_puts("\n\nhalted.");

  asm volatile(
    "hlt;"
  );
}

int do_system(int syscall_num, int one, int two, int three, int four){
  int ret = 0;
  asm (
    "mov eax, %[syscall_num];"
    // "mov esi, %[params_2];"
    "mov edi, %[params_1];"
    "mov edx, %[params_2];"
    "mov ecx, %[params_3];"
    "mov ebx, %[params_4];"
    "int 0x78;"
    "mov %[ret], eax"
    : [ret]"=m"(ret)
    : [syscall_num]"m"(syscall_num), \
      [params_1]"m"(one), \
      [params_2]"m"(two), \
      [params_3]"m"(three), \
      [params_4]"m"(four)
    : "eax", "ebx", "ecx", "edx", "edi"
  );
  return ret;
}

int get_ticks(){
  extern unsigned int ticks;
  return ticks;
}

void do_reboot(){
  outbyte(0x64, 0xfe);
}

int convert_pointer_to_global(int pointer){
  int base = PROCESS_BODY_SIZE * get_pid() + PROCESS_BODY_BASE;
  return pointer +  base;
}

int convert_pointer_to_local(int pointer){
  int base = PROCESS_BODY_SIZE * get_pid() + PROCESS_BODY_BASE;
  return pointer - base;
}

int get_pid(){
  // 获取pid
  extern unsigned int now_pro;
  return now_pro;
}

bool get_pro_status(int pid){
  extern Process process_info_table[64];
  return process_info_table[pid].status == PS_NULL ? false : true ;
}
