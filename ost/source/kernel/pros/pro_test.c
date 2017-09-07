#include "../../include/kernel/pros.h"
#include "../../include/kernel/pros_in.h"

// ---------------------------------------------

// 测试进程
// note: 在进程完成后 该模块就不再使用了

// 该模块导出：
//   pro_two

// 该模块导入：
//   无


// -------------------------------------------


void pro_test(){
  set_system;

  int a = 0;
  while(1){
    while(get_ticks() - a > 16){
      printf("__%d__", get_pid());
      a = get_ticks();
    }
    printf("__%d__", get_pid());

  }

  exit();
}
