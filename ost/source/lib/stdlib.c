#include "../include/stdlib/o_stdlib.h"

// ---------------------------------------------

// 标准库 非IO

// 该模块导出：
//   memcpy
//   memset
//   do_debug
//   set_debug_env
//   delay

// 该模块导入：
//   无


// -------------------------------------------

static void debug(char* message);

void memcpy(void* dest, void* source, int length){
  while(length){
    *(char*)dest = *(char*)source;
    dest++;
    source++;
    length--;
  }
}

void memset(void* dest, unsigned char value, int length){
  while(length){
    *(char*)dest = value;
    dest++;
    length--;
  }
}
extern void* printf;
static void debug(char* message){
  asm(
    "push %[message];"
    "call %[printf];"
    "add esp, 4;"
    :: [message]"r"(message), \
       [printf]"r"(&printf)
  );
}

void do_debug(char* message){
  asm(
    "push %[message];"
    "call 0x100000;"
    "add esp, 4;"
    :: [message]"r"(message)
  );
}
void set_debug_env(){
  memcpy((void*)0x100000, &debug, 200);
}
void delay(){
  for(int i = 0; i< 100000; i ++){
    for(int i = 0; i< 1000; i ++){
      int a = 1;
    }
  }
}
