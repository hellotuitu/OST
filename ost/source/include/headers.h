// 为方便编程 直接包含所有的库
#ifndef _HEADERS_H_

#define _HEADERS_H_

#ifndef _TYPE_H_
#include "type.h"
#endif

#include "init/gdt.h"
#include "init/idt.h"
#include "init/int_handler.h"
#include "init/protect.h"
#include "init/syscall.h"
#include "init/keyboard.h"
#include "init/acpi.h"
#include "time.h"

#include "kernel/process.h"
#include "kernel/pros.h"

#include "ostlib/ostio.h"
#include "ostlib/ostlib.h"

#include "stdlib/o_string.h"
#include "stdlib/o_stdio.h"
#include "stdlib/o_stdlib.h"

#include "init.h"

#include "kernel/driver/floppy.h"

void init_main(void);

#endif
