#include "../include/init/protect.h"
#include "../include/stdlib/o_stdlib.h"
#include "../include/init/gdt.h"

// ---------------------------------------------

// 重设gdt

// 该模块导出：
//   init_descriptor set_desc_in_gdt set_gdt

// 该模块导入：
//   back_to_realmode

// -------------------------------------------

void back_to_realmode(); // 导入的

static void* gdt_base = (void *)GDT_BASE;
static unsigned short gdt_length = GDT_LENGTH;

//初始化段描述符
void init_descriptor(Descriptor* p_desc, unsigned int base_addr, unsigned int limit, unsigned short attribute){
  p_desc -> limit_low	= limit & 0x0FFFF;		// 段界限 1		(2 字节)
  p_desc -> base_low		= base_addr & 0x0FFFF;		// 段基址 1		(2 字节)
  p_desc -> base_mid		= (base_addr >> 16) & 0x0FF;		// 段基址 2		(1 字节)
  p_desc -> attr1			= attribute & 0xFF;		// 属性 1
  p_desc -> limit_high_attr2	= ((limit >> 16) & 0x0F) |
            (attribute >> 8) & 0xF0;// 段界限 2 + 属性 2
  p_desc->base_high		= (base_addr >> 24) & 0x0FF;		// 段基址 3		(1 字节)
}

void set_desc_in_gdt(Descriptor* p_desc, int gdt_index){
  memcpy(gdt_base + gdt_index * 8, (void *)p_desc, 8);
}

static void init_gdt(){
  memset(gdt_base, 0, gdt_length * 8);
}

static void update_gdtr(){
  unsigned short gdt_limit = gdt_length * 8 - 1;
  memcpy(gdt_base + gdt_length * 8, &gdt_limit, 2);
  memcpy(gdt_base + gdt_length * 8 + 2, &gdt_base, 4);

  asm volatile(
    "mov esi, %[gdt_ptr];"
    "lgdt	[esi]"
    :
    : [gdt_ptr]"r"(gdt_base + gdt_length * 8)
    : "esi"
  );
}

void set_gdt(){
  //初始化gdt
  init_gdt();

  Descriptor desc;
  memset(&desc, 0, 8);

  // 第一个描述符为内核代码描述符
  init_descriptor(&desc, 0, 0xfffff, DA_CR | DA_32 | DA_LIMIT_4K);
  set_desc_in_gdt(&desc, 1);

  //第二个描述符为内核数据描述符
  init_descriptor(&desc, 0, 0xfffff, DA_DRW | DA_32 | DA_LIMIT_4K);
  set_desc_in_gdt(&desc, 2);

  // 第68个描述符 返回实模式的代码段
  init_descriptor(&desc, (int)back_to_realmode, 0xffff, DA_C);
  set_desc_in_gdt(&desc, 68);

  // 第69个描述符 返回实模式的数据段
  init_descriptor(&desc, (int)back_to_realmode, 0xffff, DA_DRW);
  set_desc_in_gdt(&desc, 69);

  //更新gdt寄存器
  update_gdtr();
}
