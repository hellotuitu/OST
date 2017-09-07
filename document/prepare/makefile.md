### makefile的学习笔记

#### 基本格式
    target : prerequisites ...
        command
        ...

- 说明：target是需要生成的目标文件，prerequisites是生成该目标文件的所有的依赖文件.当生成target时,程序会判断目标文件与依赖项的新旧程度，如果依赖项更新时间更晚，则执行下面的command

- 当直接输入make命令时，程序会在当前目录下寻找makefile文件，并把在makefile中找到的第一个目标文件当做最终的目标文件

#### makefile中使用变量
    objects = main.o kbd.o command.o display.o /
          insert.o search.o files.o utils.o

    edit : $(objects)
        gcc -o edit $(objects)

#### make的自动推导功能
如果make程序目标是一个.o格式的目标文件,以foo.o为例.程序会自动将foo.c加入foo.o的依赖项，并且自动推导编译命令`gcc -c foo.c`.在编写makefile文件时，只需将make无法推导出的依赖项加上即可

    main.o : defs.h
    kbd.o : defs.h command.h
    command.o : defs.h command.h
    ...

#### 清空目标文件:clean
为了便于重编译，makefile中应该有一个清空目标文件的命令，一般用`clean`来表示这个命令

- 注意，clean不是一个真正的目标，它是一个伪目标，伪目标有特殊的书写格式
- clean命令一般不能放在makefile的开头，否则clean会被当成最终的目标


    .PHONY : clean
    clean :
        -rm edit $(objects)

#### 规则中的通配符
makefile支持三种通配符`* ? [...]`

#### 文件搜寻路径
在寻找源文件时，make只会在当前目录下寻找.如果想要添加其他目录，可以指定vpath，vpath的指定方法是`vpath < pattern> < directories> `,含义是`在符合模式< pattern>的文件指定搜索目录<directories>`，vpath可多次指定

    vpath %.c foo
    vpath %   blish
    vpath %.c bar

#### 伪目标
伪目标类似一个label，其并不是真正的目标，只是作为一个标签来使用。因为伪目标并不生成文件，因此可不填写依赖关系，只拥有命令.为防止伪目标与目标文件重名，慎重的方法是为每一个伪目标加上声明，如`.PHONY: clean`。伪目标下的命令可以通过`make label`的方式来执行。

伪目标亦可以填写依赖关系，当填写依赖关系时有特殊用法.当想要通过一个目标文件生成多个目标时，可以将这多个目标声明为伪目标的依赖项，并把伪目标作为第一个目标。并且， 伪目标也可以作为依赖项使用。

    all : prog1 prog2 prog3
    .PHONY : all

    prog1 : prog1.o utils.o
          cc -o prog1 prog1.o utils.o

    prog2 : prog2.o
          cc -o prog2 prog2.o

    prog3 : prog3.o sort.o utils.o
          cc -o prog3 prog3.o sort.o utils.o

#### 预设变量
make中有三个非常有用的预设变量
- `$@`  目标文件
- `$^`  所有的依赖文件
- `$<`  第一个依赖文件
