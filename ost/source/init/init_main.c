#include "../include/init.h"

// ---------------------------------------------

// 初始化总入口

// 该模块导出：
//   init_main

// 该模块导入：
//   set_gdt set_idt

// -------------------------------------------

void init_main(){
  // 重设gdt
  set_gdt();

  // 暂时不使用分页功能 开启分页 但只做对等映射

  // 设置idt
  set_idt();

  initAcpi();

  time_init();
}
