%include	"ost_loader.inc"

org 100h

;begin
;16位引导代码

[BITS	16]
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, 0100h

	; 初始化 32 位代码段描述符
	xor	eax, eax
	mov	ax, cs
	shl	eax, 4
	add	eax, label_seg_32_code
	mov	word [label_desc_32_code + 2], ax
	shr	eax, 16
	mov	byte [label_desc_32_code + 4], al
	mov	byte [label_desc_32_code + 7], ah

  xor	eax, eax
	mov	ax, cs ; 装入当前cs
	shl	eax, 4 ; 实模式下 计算data真正的物理地址
	add	eax, label_seg_data ; 得到真正的物理地址
	mov	word [label_desc_data + 2], ax
	shr	eax, 16
	mov	byte [label_desc_data + 4], al
	mov	byte [label_desc_data + 7], ah

	; 为加载 GDTR 作准备
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, label_gdt		; eax <- gdt 基地址
	mov	dword [gdt_ptr + 2], eax	; [GdtPtr + 2] <- gdt 基地址

	; 加载 GDTR
	lgdt	[gdt_ptr]

	; 关中断
	cli

	; 打开地址线A20
	in	al, 92h
	or	al, 00000010b
	out	92h, al

	; 准备切换到保护模式
	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax

	; 真正进入保护模式
	jmp	dword selector_32_code:0	

  ; 另一种写法 直接写二进制
  ; dd 0000EA66h
  ; dd 00080000h
  ; 66EA为跳转指令
  ; 00000000 为段内偏移
  ; 0008 为选择子

[bits 32]
label_seg_data:
  message db "hello world"
  message_len equ $- message

seg_data_len equ $ - label_seg_data

label_seg_32_code:
  mov ax, selector_data
  mov ds, ax    ; 初始化数据段
  mov	ax, selector_video
  mov	gs, ax			; 视频段选择子(目的)

  mov ecx, message_len ; 循环次数
  mov edx, 800
  mov ebx, 0
print:
  mov eax, edx
  add eax, ebx
  add eax, eax
  mov	edi, eax	; 屏幕第 10 行, 第 0 列。
  mov	ah, 0Ch			; 0000: 黑底    1100: 红字
  mov	al, [ds:ebx]
  mov	[gs:edi], ax
  inc ebx
  loop print

  ; 到此停止
  jmp	$

seg_32_code_len	equ	$ - label_seg_32_code

label_gdt: Descriptor 0, 0, 0 ;空描述符
label_desc_32_code: Descriptor 0, seg_32_code_len - 1, DA_C + DA_32
label_desc_video: Descriptor 0B8000h, 0ffffh, DA_DRW ; 视频段
label_desc_data: Descriptor 0, seg_data_len - 1, DA_DRW

gdt_len equ $ - label_gdt
gdt_ptr dw	gdt_len - 1	; GDT界限
		    dd	0		; GDT基地址
selector_video equ label_desc_video - label_gdt
selector_32_code equ label_desc_32_code - label_gdt
selector_data equ label_desc_data - label_gdt
