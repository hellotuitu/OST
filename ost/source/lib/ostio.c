#include "../include/ostlib/ostio.h"
#include "../include/ostlib/ostlib.h"
#include "../include/stdlib/o_stdlib.h"

// ---------------------------------------------

// 系统底层io库

// 该模块导出：
//   ost_putchar
//   ost_puts
//   ost_colorful_putchar
//   ost_colorful_puts
//   outbyte_d
//   outbyte
//   inbyte
//   clean_screen

// 该模块导入：
//   无


// -------------------------------------------


static unsigned short get_cursor_pos(); //内部函数 获取光标位置
static void set_pos(unsigned short pos); // 内部函数 设置光标位置
// static void line_up(); //  内部函数 使上卷一行
// static void line_down(); //  内部函数 使下卷一行
#define video_mem 0xb8000
static unsigned int pos = 0; //当前光标位置

static unsigned short get_cursor_pos(){
  unsigned short pos = 0;
  asm volatile(
    "mov dx, 0x3d4;" // red word and black background
    "mov al, 0xe;"
    "out dx, al;"
    "inc dx;"
    "in al, dx;"
    "mov bh, al;" // 读取光标高八位放入bh中
    "dec dx;"
    "mov al, 0xf;"
    "out dx, al;"
    "inc dx;"
    "in al, dx;"
    "mov bl, al;" // 读取光标低八位放入bl中
    "mov %[output], bx;"
    : [output]"=r"(pos)
    :
    : "ax", "bx", "dx"
  );

  return pos;
}

static void set_cursor_pos(unsigned short pos){
  if(pos < 0 || pos > 80 * 25)
    return;
  asm volatile(
    "mov bx, %[input];"
    "mov dx, 0x3d4;"
    "mov al, 0xe;"
    "out dx, al;"
    "inc dx;"
    "mov al, bh;"
    "out dx, al;" // 写入光标高八位
    "dec dx;"
    "mov al, 0xf;"
    "out dx, al;"
    "inc dx;"
    "mov al, bl;"
    "out dx, al;"  //写入光标低八位
    :
    : [input]"r"(pos)
    : "ax", "bx", "dx"
  );
}

//使得屏幕内容向上移动一行
static void line_up(){
  memcpy((void*)video_mem, (void*)(video_mem + 80 * 2), 80 * 2 * 24);
  for(int i=0; i < 80; i++){
    memset((void*)(video_mem + 80 * 2 * 24 + i * 2), 0, 1);
  }
  // memset((void*)(video_mem + 80 * 2 * 24), 0, 80 * 2);

}
void ost_putchar(char chr){
  if(chr == '\n'){
    if((pos / 80 + 1) >= 25){
      line_up();
      pos = pos / 80 * 80; // 回到行首
    }
    else{
      pos = (pos / 80 + 1) * 80;
    }
  }
  else if(chr == 40){
    if(pos < 1)
      return;
    else{
      pos--;
      ost_putchar(0);
      pos--;
    }
  }
  else{
    char temp = chr;
    asm volatile(
      "mov	ah, 0xF;"
      "mov	al, %[input];"
      "mov  esi,%[pos];"
      "add  esi, esi;"
      "mov	word ptr gs:[0xb8000 + esi], ax;"
      :
      : [input]"r"(temp), [pos]"r"(pos)
      : "eax", "esi"
    );
    if(pos + 1 >= 80 * 25){
      line_up();
      pos = pos - 80; // 回到行首
    }
    else{
      pos++;
    }
  }
  set_cursor_pos(pos);
}

void ost_colorful_putchar(char chr, char color){
  if(chr == '\n'){
    if((pos / 80 + 1) >= 25){
      line_up();
      pos = pos / 80 * 80; // 回到行首
    }
    else{
      pos = (pos / 80 + 1) * 80;
    }
  }
  else if(chr == 40){
    //对退格的处理
    if(pos < 1)
      return;
    else{
      pos--;
      ost_putchar(0);
      pos--;
    }
  }
  else{
    char temp = chr;
    asm volatile(
      "mov	ah, %[attribute];"
      "mov	al, %[input];"
      "mov  esi,%[pos];"
      "add  esi, esi;"
      "mov	word ptr gs:[0xb8000 + esi], ax;"
      :
      : [attribute]"r"(color), [input]"r"(temp), [pos]"r"(pos)
      : "eax", "esi"
    );
    if(pos + 1 >= 80 * 25){
      line_up();
      pos = pos - 80; // 回到行首
    }
    else{
      pos++;
    }
  }

  set_cursor_pos(pos);

}
void ost_puts(char* str){
  for(int i=0; str[i]!=0; i++){
    ost_putchar(str[i]);
  }
}

void ost_colorful_puts(char* str, char color){
  for(int i=0; str[i]!=0; i++){
    ost_colorful_putchar(str[i], color);
  }
}

void clean_screen(){
  // 不能将属性字节也设为零 否则无法显示光标
  for(int i = 0; i < 80 * 25; i++){
    memset((void*)(video_mem + i * 2), 0, 1);
  }
  pos = 0;
  set_cursor_pos(pos);
}

//写端口 带延迟
void outbyte_d(unsigned short port, unsigned char value){
  asm (
    "mov dx, %[port];"
    "mov al, %[value];"
    "out dx, al;"
    "nop;"
    "nop;"
    "nop;"
    :
    : [port]"r"(port), [value]"r"(value)
    : "al", "dx"
  );
}

void outbyte(unsigned short port, unsigned char value){
  asm (
    "mov dx, %[port];"
    "mov al, %[value];"
    "out dx, al;"
    :
    : [port]"r"(port), [value]"r"(value)
    : "al", "dx"
  );
}

void outword(unsigned short port, unsigned short value){
  asm (
    "mov dx, %[port];"
    "mov ax, %[value];"
    "out dx, ax;"
    :
    : [port]"r"(port), [value]"r"(value)
    : "ax", "dx"
  );
}

char inbyte(unsigned short port){
  unsigned char ret = 0;
  asm (
    "mov al, 0;"
    "mov dx, %[port];"
    "in al, dx;"
    "mov %[value], al;"
    : [value]"=m"(ret)
    : [port]"r"(port)
    : "al", "dx"
  );

  return ret;
}

char inbyte_d(unsigned short port){
  unsigned char ret = 0;
  asm (
    "mov al, 0;"
    "mov dx, %[port];"
    "in al, dx;"
    "mov %[value], al;"
    "nop;"
    "nop;"
    "nop;"
    : [value]"=m"(ret)
    : [port]"r"(port)
    : "al", "dx"
  );

  return ret;
}

unsigned short inword(unsigned short port){
  unsigned short ret = 0;
  asm (
    "mov ax, 0;"
    "mov dx, %[port];"
    "in ax, dx;"
    "mov %[value], ax;"
    : [value]"=m"(ret)
    : [port]"r"(port)
    : "ax", "dx"
  );

  return ret;
}
