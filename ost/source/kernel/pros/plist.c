#include "../../include/kernel/pros.h"
#include "../../include/kernel/pros_in.h"


// ---------------------------------------------

// plist 列出当前的进程
// note: 该进程的实现是让该进程直接访问了进程信息表 这样极不安全 ; 已修改 

// 该模块导出：
//   plist

// 该模块导入：
//   无


// -------------------------------------------

void plist(){
  set_system;

  char header[] = "Name      Pid\n";
  char format[] = "\n%-10s%d";

  int a = 0;
  while(1){
    printf("__%d__", get_pid());
    sleep(2) ;
  }
  exit();
}
