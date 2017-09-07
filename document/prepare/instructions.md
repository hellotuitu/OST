### 16位指令与32位指令的区别

目前来看 16位与32位之间的区别在于
操作数 与 地址的 区别  操作码都是一样的
eg: EA 操作码表示远距离绝对跳转 16下与32下都相同

#### cpu如何区别指令是32位的还是16位的？

通过指令前缀 0x66 实现
        操作码 偏移       段地址        
eg: 32: 66EA   0000 0000  0800  jmp dword 0x8:0x0
    16: EA     3E04       0000  jmp word 0x0:0x43e
#### 纠正
 在实模式下，默认认为指令是16位的
 除非加了操作码前缀， 才认为是32位的
 在保护模式下， 段描述符中的d/b位用来设置该代码段默认的指令位数