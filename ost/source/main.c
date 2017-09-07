#include "./include/headers.h"

// ---------------------------------------------

// 操作系统主体

// 该模块导出：
//   main

// 该模块导入：
//   无


// -------------------------------------------

//该符号在init/realmode.asm中导出
void back_to_realmode();

//返回实模式的函数 暂时放在这里
void go_to_realmode();

void go_to_realmode(){
  //重新设置 8259a
  //该函数在init/idt.c中实现
  set_realmode_8259a();

  // 68 * 8 = 544
  asm("jmp 544:0;");
  // 跳到 16代码段处 第68个选择子
}
int main(){
  //完成os环境的设置
  clean_screen();

  init_main();
  //设置debug环境
  //该函数在0x100000处放置了一个函数
  //使用 do_debug(char*) 来调用位于0x100000处的函数
  // 使用bohcs调试时 用"b 0x100000"来下断点
  set_debug_env();

  start_process();

  return 0;
}
