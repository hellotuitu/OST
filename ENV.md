### 开发环境
### 准备阶段
#### Windows操作系统
准备阶段中使用的操作系统

#### VMWare虚拟机
##### 安装dos操作系统
下载镜像，创建虚拟机时连接到镜像
开启虚拟机后按F2进入bios 设置CD/DVD启动
安装完成后取消镜像与虚拟机的连接

##### 可能的错误
`EMM386: unrecovery ......`


出现这种错误时 注释掉config.sys文件里的有关emm的行
- 注释 ：rem
- 编辑器： edit

#### Dosbox
安装dosbox是为了快速的测试代码的正确性，但测试结果并不可信.

#### UltraISO

用来创建软盘镜像(ima)与硬盘镜像(iso).

- iso: 用于创建安装镜像等...
- ima：用于在dos和windows中传递文件.

#### Nasm
项目使用的汇编编译器

#### Atom
项目使用的编辑器

##### 使用的package
- asm*86 语法高亮
- atom-terminal 快速打开命令行

#### 代码管理
git

远程库：coding

### 开发阶段

#### linux(ubuntu 16.04)操作系统
开发环境使用的操作系统

#### VitualBox
虚拟机软件，安装dosbox用于debug以及一个无操作系统的虚拟机用于加载OST

#### Bochs
×86模拟器, 用于操作系统debug

#### mingw
c语言相关工具集

#### git
版本控制工具

#### nasm
汇编编译器

#### atom
代码编辑器

#### make(属于mingw,但鉴于其重要性，单独列出)
自动化编译工具
