//与底层实现相关的库函数

#ifndef _OSTLIB_H_

#define _OSTLIB_H_

#ifndef _TYPE_H_
#include "../type.h"
#endif

#define sleep(x) do{ \
  int a = get_ticks(); \
  while((get_ticks() - a)  < x * 100); \
}while(0)

void halt(); // 挂起整个系统.
int do_system(int syscall_num, int one, int two, int three, int four);

int get_ticks();
void do_reboot();
int convert_pointer_to_local(int);
int convert_pointer_to_global(int);
int get_pid();
bool get_pro_status(int);

#endif
