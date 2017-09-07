
.PHONY : gen debug-gen clean build run debug

CC = gcc
LD = ld

ENTRYPOINT	= 0x30400

INC = -I./source/include -I./source/include/init \
      -I./source/include/kernel -I./source/include/kernel/driver \
			-I./source/include/ostlib -I./source/include/stdlib

SRCS_C = $(wildcard ./source/*.c) \
         $(wildcard ./source/init/*.c) \
         $(wildcard ./source/init/irq/*.c)  \
         $(wildcard ./source/kernel/*.c)  \
			   $(wildcard ./source/kernel/pros/*.c) \
			   $(wildcard ./source/kernel/driver/*.c) \
			   $(wildcard ./source/lib/*.c)

SRCS_ASM = $(wildcard ./source/*.asm) \
           $(wildcard ./source/init/*.asm)

cflags = -masm=intel -m32 -c -fno-builtin -fno-stack-protector $(INC)
ldflags = -s -Ttext $(ENTRYPOINT) -m elf_i386
clean_objs = bin/boot.debug.img  bin/kernel.bin bochsout.txt \
             $(wildcard ./source/*.o) \
			       $(wildcard ./source/init/*.o) \
			       $(wildcard ./source/init/irq/*.o)  \
			       $(wildcard ./source/kernel/*.o)  \
			       $(wildcard ./source/kernel/pros/*.o) \
			       $(wildcard ./source/kernel/driver/*.o) \
			       $(wildcard ./source/lib/*.o)

objs = $(patsubst %.asm,%.o,$(SRCS_ASM)) $(patsubst %.c,%.o,$(SRCS_C)) 

bin/kernel.bin : $(objs)
	@$(LD) $(ldflags) $^ -o $@
	@echo "build kerbin.bin successful."

%.o:%.c
	$(CC) $(cflags) $< -o $@

%.o:%.asm
	nasm -f elf32 $< -o $@

build : bin/kernel.bin gen
# to generate kerbin.bin and boot.img

gen :
# to generate boot.img
	@-mkdir mount
	@dd if=/dev/zero of=bin/boot.img bs=512 count=2880 2>/dev/null
	@sudo losetup /dev/loop0 bin/boot.img 2>/dev/null
	@sudo mkfs.msdos /dev/loop0 1>/dev/null 2>/dev/null
	@dd  bs=512 if=bin/boot.bin of=/dev/loop0 count=1 2>/dev/null
	@sudo losetup -d /dev/loop0
	@sudo mount -o loop,rw bin/boot.img mount
	@sudo cp bin/loader.bin mount/
	@sudo cp bin/kernel.bin mount/
	@sudo umount mount
	@-rm -r mount
	@echo "boot.img has been created successful."

debug-gen :
# to generate boot.debug.img
	@-mkdir mount
	@dd if=/dev/zero of=bin/boot.debug.img bs=512 count=2880 2>/dev/null
	@sudo losetup /dev/loop0 bin/boot.debug.img 2>/dev/null
	@sudo mkfs.msdos /dev/loop0 1>/dev/null 2>/dev/null
	@sudo losetup -d /dev/loop0 2>/dev/null
	@sudo mount -o loop,rw bin/boot.debug.img mount
	@sudo cp bin/loader.bin mount/
	@sudo cp bin/boot.com mount/
	@sudo cp bin/kernel.bin mount/
	@sudo umount mount
	@-rm -r mount
	@echo "boot.debug.img has been created successful."

clean :
# clean objs and other files for recompile.
	@-rm -f $(clean_objs)
	@echo "clean completed."

run :
# 启动ost
	VBoxManage  startvm  OST

debug :
	bochs -q

# 如何在真实的物理机上启动操作系统？
# 方法一(windows)：
#  使用ultraiso直接写入硬盘镜像 选择raw写入方式
# 方法二(ubuntu)：
#  使用df -h命令查看已挂载设备
#  umount 已挂载设备
#  使用 dd bs=4M if=需写入的镜像路径 of=设备名称 写入数据
#  举例:
#    df -h //由此知道usb设备的路径 假设设备名称是/dev/sdb
# 	 sudo umount /dev/sdb // 卸载设备
# 	 sudo dd if=bin/boot.img of=/dev/sdb //写入数据
