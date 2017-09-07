### 内联汇编学习
以下所述内联汇编均为gcc内联汇编，且使用intel语法.内联汇编标准为C99标准.

关于最新gcc内联汇编标准，请参考[gcc内联汇编标准中文文档](https://github.com/tidyjiang8/arm-gcc-inline-assembler/blob/master/src/arm-gcc-inline-assembler.md)
#### 内联汇编基本格式
    asm (
        汇编语句;
        :输出操作数;
        :输入操作数;
        :其他被污染的寄存器
      );

1. 汇编语句为多行时用`;`或`\n\t`分隔
2. 当使用序号参数(即%0...)时,参数列表从输出依次到输入

以下为两个例子

使用序号参数(较早的内联汇编标准，不推荐使用)
```c
#include<stdio.h>

int main(){
  int a = 1, b = 2, c = 0;
  //calculate c = a + b
  asm (
    "add %0, %2"
    : "=g"(c) // 第0个参数 输出
    : "g"(a), "g"(b) // 当不指定第一个参数时 第一个参数同第0个参数
    : "memory"
  );

  printf("%d", c);
  return 0;
}
```
使用符号参数
```c
#include<stdio.h>

int main(){
  int a = 1, b = 2, c = 0;
  //calculate c = a + b;

  asm(
    "mov eax, %[params_one];"
    "add eax, %[params_two];"
    "mov %[output], eax;"
    : [output]"=g"(c)
    : [params_two]"g"(a), [params_one]"g"(b)
    : "memory"
  );

  printf("%d", c);
  return 0;
}
```

#### 在内联汇编中使用intel语法
一般来说，gcc内联汇编使用的是att语法，若要使用intel语法，则需在编译时的命令行参数中加上`-masm=intel `,参数`-masm=intel`并不总是有效，该参数可能是平台相关的，目前已知在osx平台无效，其他平台待测。

#### 避免优化错误
为避免gcc在对内联汇编的优化中发生错误，使用`volatile`关键字来指示编译器不要对该部分代码进行优化
