#include "../include/stdlib/o_string.h"

// ---------------------------------------------

// string库 不保证安全性

// 该模块导出：
//   strlen
//   strcat
//   strcmp
//   strapd
//   strcpy

// 该模块导入：
//   无


// -------------------------------------------


//求字符串长度
int strlen(char* str){
  char* dest = str;

  while(*str != 0){
    str++;
  }

  return str - dest;
}

//字符串连接函数
void strcat(char* dest, char* source){
  while(*dest != 0){
    dest++;
  }

  while(*source != 0){
    *dest = *source;
    dest++;
    source++;
  }

  *dest = 0;
}

bool strcmp(char* dest, char* source){
  while(*source != 0 || *dest != 0){
    if(*source != *dest)
      return false;
    source++;
    dest++;
  }

  return true;
}
void strcpy(char* dest, char* source){
  while(*source != 0){
    *dest = *source;
    dest++;
    source++;
  }
  *dest = 0;
}
//字符串附加字符函数
void strapd(char* dest, char chr){
  while(*dest != 0){
    dest++;
  }

  *dest = chr;
  dest++;
  *dest = 0;
}
