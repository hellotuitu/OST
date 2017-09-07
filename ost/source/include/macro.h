//some assembly macros like io_read and io_write
#ifndef _MACRO_H_

#define _MACRO_H_

#ifndef _TYPE_H_
#include "type.h"
#endif

// 输出到端口 无延迟
#define outbyte(port, value) \
  asm ( \
    "mov dl, %[value];" \
    "mov al, %[port];" \
    "out al, dl" \
    : \
    : [port]"r"(port), [value]"r"(value) \
    : \
  )

// 输出到端口 延迟
#define outbyte_d(port, value)  \
  asm ( \
    "mov dx, %[port];" \
    "mov al, %[value];" \
    "out dx, al;" \
    "nop;" \
    "nop;" \
    "nop;" \
    : \
    : [port]"r"(port), [value]"r"(value) \
    : "al", "dx" \
  );

 //从端口读入 无延迟
#define inbyte(port)({ \
  unsigned char _v; \
  asm ( \
    "mov al, %[port];" \
    "in dx, al;" \
    "mov %[output], dx;" \
    : [output]"=r"(_v) \
    : [port]"r"(port) \
  ); \
  _v; \
})

//从端口读入 延迟
#define inbyte_d(port)({ \
  unsigned char _v; \
  asm ( \
    "mov al, %[port];" \
    "in dx, al;" \
    "mov %[output], dx;" \
    "nop;" \
    "nop;" \
    "nop;" \
    : [output]"=r"(_v) \
    : [port]"r"(port) \
  ); \
  _v; \
})

#endif
