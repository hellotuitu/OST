org  0100h
segment .data
  HELLO db "i hate assemblely code...", 0
  WORLD db "the second string", 0
  AWORD dw 00000h
segment .bss
segment .text
  global _asm_main
  _asm_main:
    enter 0,0 ; 开始运行
    pusha
    push HELLO
    call print
    push WORLD
    call print
    push HELLO
    call println
    push AWORD
    call print_word_b
    push AWORD
    call print_word_h
    ; mov al, 10
    ; mov ah, 6
    ; mov cx, 0
    ; mov dh, 80
    ; mov dl, 80
    ; int 10h
    ; mov ah,0fh
    ; int 10h
    ; mov ah, 5h
    ; inc bh
    ; mov al, bh
    ; int 10h
    leave
    ret

  print:
    push bp
    mov bp, sp
    mov cx, 0  ;store the chars of string
    mov bx, [bp+4]
  count:
    cmp byte [bx], 0
    jz print_char
  add_char:
    inc bx
    inc cx
    jmp count
  print_char:
    push bp
    push cx
    mov bp, [bp+4]
    mov ax, 0f00h   ; 功能号 0f
    int 10h         ; 读取页号 存放在 bh中
    mov ax, 0300h   ;
    mov bl, 0       ; ah 功能号 bh 页码
    int 10h         ; 查询当前光标位置 dh  行号  dl 列号
    pop cx
    mov	ax, 1301h
    mov	bx, 000ch		; 页号为0(BH = 0) 黑底红字(BL = 0Ch,高亮)
	  int	10h			    ; 显示字符
	  pop bp
	  leave
	  ret 2

	 println:
	   push bp
	   mov bp, sp
	   mov ax, [bp+4]
	   pusha
	   push ax
	   call print
	   popa
	   mov ax, 0f00h   ; 功能号 0f
     int 10h         ; 读取页号 存放在 bh中
	   mov ax, 0300h
     mov bl, 0
     int 10h         ; 读取光标
     mov ax, 0200h
     inc dh
     mov dl, 0
     int 10h         ; 设置光标
     leave
	   ret 2

	 print_word_b:
	   push bp
	   mov bp, sp
	   sub sp, 2     ; 复制参数
	   mov si, [bp+4]
	   mov di, sp
	   mov ax, [si]
	   mov [di], ax
	   sub sp, 18
	   mov bx, bp
	   sub bx, 4
	   mov byte [bx], 0
	   sub bx, 16
	   mov cx, 16
	   mov si, [bp-2]
	 convert:
	   shl word si, 1
	   jc binery_one
	   mov byte [bx], 48
	   inc bx
	   jmp circle
	 binery_one:
	   mov byte [bx], 49
	   inc bx
	 circle:
	   loop convert ; the loop time
	   mov ax, bp
	   sub ax, 20
	   push ax
	   call println
	   leave
	   ret 2

	 print_word_h:
	   push bp
	   mov bp, sp
	   sub sp, 2     ; 复制参数
	   mov si, [bp+4]
	   mov di, sp
	   mov ax, [si]
	   mov [di], ax  ; 复制完毕
	   sub sp, 6     ; 存放word+0
	   mov cx, 4     ; 循环四次
	   mov ax, 000fh
	   mov si, bp
	   sub si, 4
	   mov byte [si], 0 ; 结束标志 0
	   sub si, 1        ; si 作为指向字符串的指针
	   mov di, bp
	   sub di, 2
	 convert_word:
	   mov dx, [di]
	   and dx, ax
	   shr word [di], 4
	   cmp dx, 10
	   jnc big_than_ten
	 small_than_ten:
	   add dx, 48  ; 加48 变成asii
	   jmp store
	 big_than_ten:
	   add dx, 55  ; 加55 变成asii
	 store:
	   mov [si], dl
	   sub si, 1
	   loop convert_word
	   mov si, bp
	   sub si, 8
	   push si
	   call println
	   leave
	   ret 2
