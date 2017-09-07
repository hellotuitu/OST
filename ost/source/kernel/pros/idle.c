#include "../../include/kernel/pros.h"
#include "../../include/kernel/pros_in.h"


// ---------------------------------------------

// idle进程

// 该模块导出：
//   idle

// 该模块导入：
//   无


// -------------------------------------------

//系统闲置进程
void idle(){
  //do nothing
  int (* system)(int, int, int, int, int);
  system = 0;
  // int a = 0;
  while(1){
    // pro_delay();
    // system(SYS_PRINTF, (int)"__%d__", a, 0, 0);

    // a++;
  }
}
