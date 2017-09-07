%include "source/include/init/int.inc"

; // ---------------------------------------------
;
; // 所有中断的直接入口
;
; // 该模块导出：
; //   所有中断的入口函数
; //   enable_irq disable_irq
;
; // 该模块导入：
; //   syscall_table irq_table
;
; // -------------------------------------------

 ; 关于中断处理的思路
 ; 为统一处理中断 在进程表中有一个retaddr项
 ; 首先在中断入口处调用save(此时返回地址保存在了retaddr里)
 ; 在save中会判断当前是否处于内核态中
 ; 如果在用户态 则保存完寄存器后 切换至内核栈
 ; 否则不切换堆栈
 ; 从save中返回之后
 ; 屏蔽当前正在处理的中断
 ; 接着开中断
 ; 调用相对应的中断处理程序

 ; 如何判断是否处于内核态？
 ; 当进入一个中断处理程序后 对重入变量加一
 ; 当处理完毕该中断后 对重入变量减一

 ; 中断处理分为顶端，中部， 尾部
 ; 顶端和尾端不可重入，中部可重入

 ; 尾部分两种
 ; 如果进入中断时在内核态 直接恢复寄存器
 ; 否则要加载ldt 和 更改 esp为进程任务指针

; --------------------------------------------------------
; 主中断
ALIGN	16
hwint00:		;时钟中断
  hwint_master	0

ALIGN	16
hwint01:		; Interrupt routine for irq 1 (keyboard)
	hwint_master	1

ALIGN	16
hwint02:		; Interrupt routine for irq 2 (cascade!)
	hwint_master	2

ALIGN	16
hwint03:		; Interrupt routine for irq 3 (second serial)
	hwint_master	3

ALIGN	16
hwint04:		; Interrupt routine for irq 4 (first serial)
	hwint_master	4

ALIGN	16
hwint05:		; Interrupt routine for irq 5 (XT winchester)
	hwint_master	5

ALIGN	16
hwint06:		; Interrupt routine for irq 6 (floppy)
	hwint_master	6

ALIGN	16
hwint07:		; Interrupt routine for irq 7 (printer)
	hwint_master	7

; 从中断
ALIGN	16
hwint08:		; Interrupt routine for irq 8 (realtime clock).
	hwint_slave	8

ALIGN	16
hwint09:		; Interrupt routine for irq 9 (irq 2 redirected)
	hwint_slave	9

ALIGN	16
hwint10:		; Interrupt routine for irq 10
	hwint_slave	10

ALIGN	16
hwint11:		; Interrupt routine for irq 11
	hwint_slave	11

ALIGN	16
hwint12:		; Interrupt routine for irq 12
	hwint_slave	12

ALIGN	16
hwint13:		; Interrupt routine for irq 13 (FPU exception)
	hwint_slave	13

ALIGN	16
hwint14:		; Interrupt routine for irq 14 (AT winchester)
	hwint_slave	14

ALIGN	16
hwint15:		; Interrupt routine for irq 15
	hwint_slave	15


; -------------------------------------------------------------------------
; 中断和异常 -- 异常
divide_error:
	push	0xFFFFFFFF	; no err code
	push	0		; vector_no	= 0
	jmp	exception
single_step_exception:
	push	0xFFFFFFFF	; no err code
	push	1		; vector_no	= 1
	jmp	exception
nmi:
	push	0xFFFFFFFF	; no err code
	push	2		; vector_no	= 2
	jmp	exception
breakpoint_exception:
	push	0xFFFFFFFF	; no err code
	push	3		; vector_no	= 3
	jmp	exception
overflow:
	push	0xFFFFFFFF	; no err code
	push	4		; vector_no	= 4
	jmp	exception
bounds_check:
	push	0xFFFFFFFF	; no err code
	push	5		; vector_no	= 5
	jmp	exception
inval_opcode:
	push	0xFFFFFFFF	; no err code
	push	6		; vector_no	= 6
	jmp	exception
copr_not_available:
	push	0xFFFFFFFF	; no err code
	push	7		; vector_no	= 7
	jmp	exception
double_fault:
	push	8		; vector_no	= 8
	jmp	exception
copr_seg_overrun:
	push	0xFFFFFFFF	; no err code
	push	9		; vector_no	= 9
	jmp	exception
inval_tss:
	push	10		; vector_no	= A
	jmp	exception
segment_not_present:
	push	11		; vector_no	= B
	jmp	exception
stack_exception:
	push	12		; vector_no	= C
	jmp	exception
general_protection:
	push	13		; vector_no	= D
	jmp	exception
page_fault:
	push	14		; vector_no	= E
	jmp	exception
copr_error:
	push	0xFFFFFFFF	; no err code
	push	16		; vector_no	= 10h
	jmp	exception

exception:
  mov ax, ss
  mov ds, ax
  mov es, ax
  mov gs, ax
  mov fs, ax
	call	exception_handler
	add	esp, 4*2	; 让栈顶指向 EIP，堆栈中从顶向下依次是：EIP、CS、EFLAGS
	hlt

; --------------------------------------------------------------------------
; 自定义中断

; 系统调用
sys_call:
  call	save

  sti

  ;#参数入栈 最多四个参数
  push ebx
  push ecx
  push edx
  push edi
  call	[syscall_table + eax*4]
  mov [esi + 40], eax

  add esp, 16

  cli

  ret

;用于debug的中断
;目前未被使用
debug_int:
  iret

; ----------------------------------------------------------------------------
; 中断处理共用的函数
save:
  push eax
  push ecx
  push edx
  push ebx
  push ebp
  push esi
  push edi
  push ds
  push es
  push fs
  push gs

  push eax

  mov ax, ss
  mov ds, ax
  mov es, ax
  mov gs, ax
  mov fs, ax

  pop eax

  mov esi, esp  ; 保存此时栈的位置
  inc dword [re_enter]
  cmp dword [re_enter], 1
  jne .1 ; 说明进入中断时在内核态
  ; 否则在用户态
  mov esp, 0xf0000 ; 切换内核栈
  push recover_user_mode
  jmp [esi + 4 * 11]
.1:
  push recover_kernel_mode
  jmp [esi + 4 * 11]

recover_user_mode:
  lldt  [process_ldt_selector]
  mov esp, [process_task_pointer]
recover_kernel_mode:
  dec dword [re_enter]
  pop	gs
  pop	fs
  pop	es
  pop	ds
  pop edi
  pop esi
  pop ebp
  pop ebx
  pop edx
  pop ecx
  pop eax
  add	esp, 4
  iretd

disable_irq:
  mov	ecx, [esp + 4]		; irq
  pushf
  cli
  mov	ah, 1
  rol	ah, cl			; ah = (1 << (irq % 8))
  cmp	cl, 8
  jae	disable_8		; disable irq >= 8 at the slave 8259
disable_0:
  in	al, INT_M_CTLMASK
  test	al, ah
  jnz	dis_already		; already disabled?
  or	al, ah
  out	INT_M_CTLMASK, al	; set bit at master 8259
  popf
  mov	eax, 1			; disabled by this function
  ret
disable_8:
  in	al, INT_S_CTLMASK
  test	al, ah
  jnz	dis_already		; already disabled?
  or	al, ah
  out	INT_S_CTLMASK, al	; set bit at slave 8259
  popf
  mov	eax, 1			; disabled by this function
  ret
dis_already:
  popf
  xor	eax, eax		; already disabled
  ret

enable_irq:
  mov	ecx, [esp + 4]		; irq
  pushf
  cli
  mov	ah, ~1
  rol	ah, cl			; ah = ~(1 << (irq % 8))
  cmp	cl, 8
  jae	enable_8		; enable irq >= 8 at the slave 8259
enable_0:
  in	al, INT_M_CTLMASK
  and	al, ah
  out	INT_M_CTLMASK, al	; clear bit at master 8259
  popf
  ret
enable_8:
  in	al, INT_S_CTLMASK
  and	al, ah
  out	INT_S_CTLMASK, al	; clear bit at slave 8259
  popf
  ret
