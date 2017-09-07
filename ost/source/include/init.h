//系统初始化
#ifndef _INIT_H_

#define _INIT_H_

#ifndef _TYPE_H_
#include "type.h"
#endif

#include "ostlib/ostio.h"
#include "ostlib/ostlib.h"
#include "stdlib/o_string.h"
#include "stdlib/o_stdio.h"
#include "init/gdt.h"
#include "init/idt.h"

void init_main(void);

#endif
