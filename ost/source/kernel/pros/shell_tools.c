#include "../../include/kernel/pros.h"
#include "../../include/kernel/pros_in.h"

// ---------------------------------------------

// shell使用的工具 非内建支持 通过进程实现

// 该模块导出：
//   clear 清屏
//   reboot 重启

// 该模块导入：
//   无


// -------------------------------------------

void clear(){
  set_system;

  clear_s();
  exit();
}

void reboot(){
  set_system;

  reboot_s();
  exit();
}

void time(){
  set_system;

  Time time;

  get_time(p(&time));
  printf("%d:%d:%d  ", time.tm_hour, time.tm_min, time.tm_sec);
  printf("%d/%d  %d", time.tm_mon, time.tm_mday, time.tm_year);

  exit();
}

void shutdown(){
  set_system;

  shutdown_s();
  exit();
}
