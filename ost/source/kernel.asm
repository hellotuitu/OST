[section .text]

; // ---------------------------------------------
;
; // 操作系统入口
;
; // 该模块导出：
; //   无
;
; // 该模块导入：
; //   main
;
;
; // -------------------------------------------

global _start
extern main

_start:
  ;重设堆栈
  ;重设各段寄存器
  mov ax, 16; 16是数据段的段选择子
  mov ds, ax
  mov es, ax
  mov gs, ax
  mov ss, ax
  mov esp, 050000h
  call main
