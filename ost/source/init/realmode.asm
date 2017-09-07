; // ---------------------------------------------
;
; // 由保护模式返回实模式
; // note: 能成功返回实模式 但无法使用bios中断
;
; // 该模块导出：
; //   back_to_realmode
;
; // 该模块导入：
; //   无
;
; // -------------------------------------------

global back_to_realmode

[bits 16]
ALIGN 16

back_to_realmode:
  mov ax, 69 * 8 ; 69是符合实模式的段描述符的index
  mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax

	mov	eax, cr0
  and eax, 7ffeh ; 关 分页 和 保护模式
	mov	cr0, eax

  mov eax, realmode
  sub eax, 30400h
  mov ebx, label_go_back_to_real + 1
  sub ebx, 30400h
  push word 3040h
  pop ds
  mov [ebx], ax

label_go_back_to_real:
	jmp	3040h:0	; 偏移会在back_to_realmode中被设置成正确的值

realmode:
 ; 实模式入口
  mov	ax, cs
  mov	ds, ax
  mov	es, ax
  mov	ss, ax

 ; 清零这些寄存器高地址的部分
  xor esp, esp
  xor ebp, ebp
  xor esi, esi
  xor edi, edi

  ; 堆栈
  mov sp, 0FFFFh

  in	al, 92h
	and	al, 11111101b	; 关闭 A20 地址线
	out	92h, al

	sti			; 开中断

  ; 只是halt 什么都不做
  hlt
