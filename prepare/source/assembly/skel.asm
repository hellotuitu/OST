segment .data
  HELLO db "i am the king", 0
segment .bss
segment .text
  global _asm_main
  _asm_main:
    enter 0,0 ; 开始运行
    pusha

    mov eax, 0
    leave
    ret
