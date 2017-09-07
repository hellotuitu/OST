;add a call gate for print
;add a call gate for 16_code
;manage code
;realmode->protect_mode->setup_paing->ldt_code->ring3->call_gate->backtorealmode
%include	"ost_loader.inc"

org 100h

;begin
[BITS	16]
seg_16_stack:
  times 20h db 0
seg_16_code:
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, 120h ; 建立堆栈， 方便初始化描述符

	; 初始化 32 位代码段描述符
	mov edx, label_seg_32_code
	mov esi, label_desc_32_code
	call load_desc

	mov edx, label_seg_data
	mov esi, label_desc_data
	call load_desc

	mov edx, label_seg_stack
	mov esi, label_desc_stack
	call load_desc

  mov edx, label_seg_16_code
	mov esi, label_desc_16_code
	call load_desc

  mov edx, label_ldt
  mov esi, label_desc_ldt
  call load_desc

  mov edx, label_seg_32_codeforldt
  mov esi, label_desc_32_codeforldt
  call load_desc

  mov edx, label_seg_32_codeforcgate
  mov esi, label_desc_32_codeforcgate
  call load_desc

  mov edx, label_seg_32_codeforring3
  mov esi, label_desc_32_codeforring3
  call load_desc

  mov edx, label_seg_stackforring3
  mov esi, label_desc_stackforring3
  call load_desc

  mov edx, label_seg_tss
  mov esi, label_desc_tss
  call load_desc

  mov edx, label_seg_setup_paing
  mov esi, label_desc_setup_paging
  call load_desc

  mov edx, label_seg_print
  mov esi, label_desc_print
  call load_desc

	; 为加载 GDTR 作准备
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, label_gdt		; eax <- gdt 基地址
	mov	dword [gdt_ptr + 2], eax	; [GdtPtr + 2] <- gdt 基地址

	; 加载 GDTR
	lgdt	[gdt_ptr]

  ; 将正确的段地址写入返回实模式的跳转中
  mov ax, cs
	mov [label_go_back_to_real + 3], ax

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
  load_desc:
	  ; edx 放代码偏移
		; esi 放段描述符
		xor	eax, eax
		mov	ax, cs ; 装入当前cs
		shl	eax, 4 ; 实模式下 计算段真正的物理地址
		add	eax, edx ;
		mov	word [esi + 2], ax
		shr	eax, 16
		mov	byte [esi + 4], al
		mov	byte [esi + 7], ah
		ret
label_real_code:
  mov	ax, cs
  mov	ds, ax
  mov	es, ax
  mov	ss, ax

  mov	sp, 120h

  in	al, 92h
  and	al, 11111101b	; 关闭 A20 地址线
  out	92h, al

  sti			; 开中断

  mov	ax, 4c00h
  int	21h		; 回到 DOS

label_seg_16_code:
  mov	ax, selector_realmode
  mov	ds, ax
  mov	es, ax
  mov	fs, ax
  mov	gs, ax
  mov	ss, ax

  mov	eax, cr0
  ; and	al, 011111110b
  and eax, 7ffeh ; close paging and protect mode
  mov	cr0, eax

label_go_back_to_real:
  jmp 0: label_real_code	; 段地址会在程序开始处被设置成正确的值
seg_16_code_len	equ	$ - label_seg_16_code

[bits 32]
label_seg_tss:
		DD	0			; Back
		DD	stack_top		; 0级堆栈
		DD	selector_stack		;
		DD	0			;  1级堆栈
		DD	0			;
		DD	0			;  2级堆栈
		DD	0			;
		DD	0			; CR3
		DD	0			; EIP
		DD	0			; EFLAGS
		DD	0			; EAX
		DD	0			; ECX
		DD	0			; EDX
		DD	0			; EBX
		DD	0			; ESP
		DD	0			; EBP
		DD	0			; ESI
		DD	0			; EDI
		DD	0			; ES
		DD	0			; CS
		DD	0			; SS
		DD	0			; DS
		DD	0			; FS
		DD	0			; GS
		DD	0			; LDT
		DW	0			; 调试陷阱标志
		DW	$ - label_seg_tss + 2	; I/O位图基址
		DB	0ffh			; I/o位图结束标志
seg_tss_len		equ	$ - label_seg_tss

label_seg_stack:
  times 512 db 0
seg_stack_len equ $ - label_seg_stack
stack_top equ $ - label_seg_stack - 1
label_seg_stackforring3:
  times 512 db 0
  seg_stackforring3_len equ $ - label_seg_stackforring3
  stackforring3_top equ $ - label_seg_stackforring3 - 1
label_seg_data:
  message db "hello world, i am in protect mode!"
  message_len equ $- message
  message_ldt db "hello world, i am in ldt!"
  message_ldt_len equ $ - message_ldt
  message_cgate db "hello world, i am in cgate!"
  message_cgate_len equ $ - message_cgate
  message_ring3 db "hello world, i am in ring3!"
  message_ring3_len equ $ - message_ring3
  message_paging db "hello world, i am opening paging!"
  message_paging_len equ $ - message_paging
seg_data_len equ $ - label_seg_data

label_seg_32_code:
  mov ax, selector_stack
	mov ss, ax             ; 初始化堆栈段
	mov esp, stack_top
  mov ax, selector_data
  mov ds, ax             ; 初始化数据段
  mov	ax, selector_video
  mov	gs, ax			       ; 视频段选择子(目的)

  call selector_setup_paging:0

  mov ebx, message - label_seg_data
  mov ecx, message_len
  mov edx, 80*10 + 0
  call selector_print:0

  mov ax, selector_ldt
  lldt ax

  mov ax, selector_tss
  ltr ax

  jmp selector_32_codeforldt:0

seg_32_code_len	equ	$ - label_seg_32_code

label_seg_32_codeforldt:
  mov ebx, message_ldt - label_seg_data
  mov ecx, message_ldt_len
  mov edx, 80*11
  call selector_print:0

  ; jump to ring3 by retf
  push selector_stackforring3
  push stackforring3_top
  push selector_32_codeforring3
  push 0
  retf

seg_32_codeforldt_len equ $ - label_seg_32_codeforldt
label_seg_32_codeforcgate:
  mov ebx, message_cgate - label_seg_data
  mov ecx, message_cgate_len
  mov edx, 80*13 + 0
  call selector_print:0

  ; return back to caller
  retf
seg_32_codeforcgate_len equ $ - label_seg_32_codeforcgate

label_seg_32_codeforring3:
  mov ebx, message_ring3 - label_seg_data
  mov ecx, message_ring3_len
  mov edx, 80*12 + 0
  call selector_cgate_print:0

  call selector_cgate:0

  call selector_cgate_16_code:0
  seg_32_codeforring3_len equ $ - label_seg_32_codeforring3

label_seg_setup_paing:
  mov ebx, message_paging - label_seg_data
  mov ecx, message_paging_len
  mov edx, 80*14
  call selector_print:0

  mov ax, selector_pde
  mov es, ax
  mov ecx, 1024 ; 1024 times circle
  xor eax, eax
  xor edi, edi
  mov eax, pte_base_adress | PG_P | PG_RWW | PG_USU
  .1:
    stosd ; mov eax to es:edi , and inc edi or dec edi edpends on flags
    add eax, 4096 ; next pte
    loop .1

  mov ax, selector_pte
  mov es, ax
  mov ecx, 1024*1024
  xor eax, eax
  xor edi, edi
  mov eax, PG_P | PG_RWW | PG_USU
  .2:
    stosd
    add eax, 4096
    loop .2

  xor eax, eax
  mov	eax, pde_base_adress
  mov	cr3, eax
  mov	eax, cr0
  or	eax, 80000000h
  mov	cr0, eax

  jmp	short .3
  .3:
    nop
    nop

  retf
seg_setup_paing_len equ $ - label_seg_setup_paing

label_seg_print:
  ; assume the data stored in data segment
  ; assume the data offset stored in ebx
  ; assume the length stored in ecx
  ; asuume the begin place stored in edx
  xor eax, eax
 print_hi:
  mov edi, edx
  add edi, eax
  add edi, edi
  push eax
  mov	ah, 0Ch			; 0000: 黑底    1100: 红字
  mov	al, [ds:ebx]
  mov	[gs:edi], ax
  pop eax
  inc ebx
  inc eax
  loop print_hi

  retf
seg_print_len equ $ - label_seg_print

label_gdt: Descriptor 0, 0, 0 ;空描述符
label_desc_realmode: Descriptor	0, 0ffffh, DA_DRW ; DA_DRW
label_desc_32_code: Descriptor 0, seg_32_code_len - 1, DA_C + DA_32 + DA_DPL0
label_desc_video: Descriptor 0B8000h, 0ffffh, DA_DRW + DA_DPL3; 视频段
label_desc_data: Descriptor 0, seg_data_len - 1, DA_DRW + DA_DPL3
label_desc_stack: Descriptor 0, seg_stack_len - 1, DA_DRWA + DA_32
label_desc_stackforring3: Descriptor 0, seg_stackforring3_len - 1, DA_DRWA + DA_32 + DA_DPL3
label_desc_16_code: Descriptor 0, 0ffffh, DA_C
label_desc_ldt: Descriptor 0, 0ffffh, DA_LDT; LDT desc in GDT
label_desc_32_codeforcgate: Descriptor 0, seg_32_codeforcgate_len, DA_C + DA_32
selector_32_codeforcgate equ label_desc_32_codeforcgate - label_gdt
label_desc_cgate: Gate selector_32_codeforcgate, 0, 0, DA_386CGate + DA_DPL3
label_desc_32_codeforring3: Descriptor 0, seg_32_codeforring3_len - 1, DA_C + DA_32 + DA_DPL3
label_desc_tss: Descriptor 0, seg_tss_len - 1, DA_386TSS
label_desc_setup_paging:Descriptor  0, seg_setup_paing_len - 1, DA_C + DA_32 + DA_DPL0
label_desc_pde: Descriptor  pde_base_adress, 4096 - 1, DA_DRW
label_desc_pte: Descriptor  pte_base_adress, 1024 -1, DA_DRW | DA_LIMIT_4K
label_desc_print: Descriptor 0, seg_print_len, DA_C + DA_32
selector_print equ label_desc_print - label_gdt
label_desc_cgateforprint: Gate selector_print, 0, 0, DA_386CGate + DA_DPL3
selector_16_code equ label_desc_16_code - label_gdt
label_desc_cgatefor16_code: Gate selector_16_code, 0, 0, DA_386CGate + DA_DPL3
gdt_len equ $ - label_gdt
gdt_ptr dw	gdt_len - 1	; GDT界限
		    dd	0		; GDT基地址
selector_video equ label_desc_video - label_gdt
selector_32_code equ label_desc_32_code - label_gdt
selector_data equ label_desc_data - label_gdt
selector_stack equ label_desc_stack - label_gdt
selector_stackforring3 equ label_desc_stackforring3 - label_gdt + SA_RPL3
selector_realmode equ label_desc_realmode - label_gdt
selector_ldt equ label_desc_ldt - label_gdt
selector_cgate equ label_desc_cgate - label_gdt + SA_RPL3
selector_32_codeforring3 equ label_desc_32_codeforring3 - label_gdt + SA_RPL3
selector_tss equ label_desc_tss - label_gdt
selector_setup_paging equ label_desc_setup_paging - label_gdt
selector_pde equ label_desc_pde - label_gdt
selector_pte equ label_desc_pte - label_gdt
selector_cgate_print equ label_desc_cgateforprint - label_gdt + SA_RPL3
selector_cgate_16_code equ label_desc_cgatefor16_code - label_gdt + SA_RPL3
label_ldt: Descriptor 0, 0, 0 ;空描述符
label_desc_32_codeforldt: Descriptor 0, seg_32_codeforldt_len, DA_C + DA_32
; ldt_len equ $ - label_ldt
; ldt_ptr dw ldt_len -1
        ; dd 0
selector_32_codeforldt: equ label_desc_32_codeforldt - label_ldt + SA_TIL
