//与底层实现无关的库函数
#ifndef _OSTDLIB_H_

#define _OSTDLIB_H_

#ifndef _TYPE_H_
#include "../type.h"
#endif

void memcpy(void*, void*, int);
void memset(void*, unsigned char, int);
void delay();
void set_debug_env();
void do_debug(char*);

#endif
