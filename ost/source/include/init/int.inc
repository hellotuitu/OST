; ------------------------------------------------------------------------
;#定义的常量
INT_M_CTL	equ	0x20
INT_M_CTLMASK	equ	0x21
INT_S_CTL	equ	0xA0
INT_S_CTLMASK	equ	0xA1

EOI		equ	0x20

; ------------------------------------------------------------------------
;#导出的符号

global	divide_error
global	single_step_exception
global	nmi
global	breakpoint_exception
global	overflow
global	bounds_check
global	inval_opcode
global	copr_not_available
global	double_fault
global	copr_seg_overrun
global	inval_tss
global	segment_not_present
global	stack_exception
global	general_protection
global	page_fault
global	copr_error
global	hwint00
global	hwint01
global	hwint02
global	hwint03
global	hwint04
global	hwint05
global	hwint06
global	hwint07
global	hwint08
global	hwint09
global	hwint10
global	hwint11
global	hwint12
global	hwint13
global	hwint14
global	hwint15

global sys_call
global debug_int
global disable_irq
global enable_irq
; --------------------------------------------------------------------------
;#导入的符号
; extern clock_handler

extern exception_handler
extern re_enter
extern process_task_pointer
extern process_ldt_selector
extern irq_table
extern syscall_table

; ----------------------------------------------------------------------------
;#使用的宏

%macro	hwint_master	1
  call	save
	; 屏蔽当前中断
  in	al, INT_M_CTLMASK
  or	al, (1 << %1)
  out	INT_M_CTLMASK, al
	; 发送中断结束信号
  mov	al, EOI
  out	INT_M_CTL, al
	; 开中断
  sti
	; 调用相应的中断处理程序
  push	%1
  call	[irq_table + 4 * %1]
  pop	ecx
  cli
	; 重新接受当前中断
  in	al, INT_M_CTLMASK
  and	al, ~(1 << %1)
  out	INT_M_CTLMASK, al
	; 返回至recover处
  ret
%endmacro

%macro	hwint_slave	1
	push	%1
	; call	spurious_irq
	add	esp, 4
	hlt
%endmacro
