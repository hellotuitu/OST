//与底层实现无关的io函数
#ifndef _OSTDIO_H_

#define _OSTDIO_H_

#include <stdarg.h>

#ifndef _TYPE_H_
#include "../type.h"
#endif

//使用变参函数出现栈检查错误， 添加以下声明解决
void __stack_chk_fail(void);

//字符串打印函数
int printf(const char*,...);
int vsprintf(char*, const char*, va_list args);

#endif
