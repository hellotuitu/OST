#include "../include/kernel/process.h"
#include "../include/kernel/pros.h"

// ---------------------------------------------

// 进程的实现

// 该模块导出：
//   process_head_table
//   process_info_table
//   函数: create_process start_process exit_process
//   note: start_process 只能被内核调用 用于开始进程调度

// 该模块导入：
//   无


// -------------------------------------------


void* process_head_table = (void*)PROCESS_HEAD_BASE;
static void* process_table = (void*)PROCESS_BODY_BASE;

Process process_info_table[64];

//进程表的结构
// 0 - 67   // 寄存器 stack_frame 68字节
// 68 - 83  // ldt
// 每一个进程表项1024字节

static void init_stack_frame(int pro_index){
  //完成寄存器的设置
  //stack_frame地址的求法
  // addr = pro_index * 1kb + PROCESS_HEAD_BASE
  Stack_Frame* sf = (Stack_Frame*)(PROCESS_HEAD_BASE + pro_index * PROCESS_HEAD_SIZE);
  memset(sf, 0, sizeof(Stack_Frame));
  sf -> cs =  ((8 * 0) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_USER;
  sf -> ds =  ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_USER;
  sf -> ss =  ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_USER;
  sf -> gs =  ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_USER;
  sf -> es =  ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_USER;
  sf -> fs =  ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_USER;
  sf -> eip = PROCESS_RESERVED_MEMORRY; //进程空间开始处
  sf -> esp = PROCESS_BODY_SIZE; // 进程空间最顶端
  sf -> ebp = PROCESS_BODY_SIZE; // 进程空间最顶端
  sf -> eflags = 0x1202; // 置中断允许位 且 允许访问端口
}
static void init_ldt(int pro_index){
  //初始化ldt 并将ldt的选择子添加到gdt
  void* ldt_base = (void*)(PROCESS_HEAD_BASE + pro_index * PROCESS_HEAD_SIZE + sizeof(Stack_Frame));

  Descriptor desc;
  memset(&desc, 0, sizeof(Descriptor));

  //ldt中的描述符
  // 为方便调用函数 暂时设置段与全局段一样
  // init_descriptor(&desc, PROCESS_BODY_BASE + pro_index * 1024 * 64, 1024 * 64 - 1, DA_CR | DA_32 | DA_DPL3);
  // init_descriptor(&desc, 0, 0xfffff, DA_CR | DA_32 | DA_DPL3 | DA_LIMIT_4K);
  init_descriptor(&desc, PROCESS_BODY_BASE + pro_index * PROCESS_BODY_SIZE, PROCESS_BODY_SIZE, DA_CR | DA_32 | DA_DPL3);
  memcpy(ldt_base, &desc, sizeof(Descriptor));
  // init_descriptor(&desc, PROCESS_BODY_BASE + pro_index * 1024 * 64, 1024 * 64 - 1, DA_DRWA | DA_32 | DA_DPL3);
  // init_descriptor(&desc, 0, 0xfffff, DA_DRWA | DA_32 | DA_DPL3 | DA_LIMIT_4K);
  init_descriptor(&desc, PROCESS_BODY_BASE + pro_index * PROCESS_BODY_SIZE, PROCESS_BODY_SIZE, DA_DRWA | DA_32 | DA_DPL3);
  memcpy(ldt_base + 8, &desc, sizeof(Descriptor));

  //ldt的段描述符
  init_descriptor(&desc, (int)ldt_base, 0xffff, DA_LDT);
  set_desc_in_gdt(&desc, pro_index + 4);
}

void init_global_tss(){
  //初始化全局共用的tss
  Tss* tss = (Tss*)(PROCESS_GLOBAL_TSS);
  memset(tss, 0, sizeof(Tss));
  tss -> ss0 = 16;
  //初始化为进程0的stack_frame顶端
  //这意味着第一个启动的进程必然是进程0
  tss -> esp0 = PROCESS_HEAD_BASE + sizeof(Stack_Frame);
  // 无io位图
  tss -> iobase	= sizeof(Tss);

  Descriptor desc;
  memset(&desc, 0, sizeof(Descriptor));

  init_descriptor(&desc, (unsigned int)(tss), sizeof(Tss) - 1 , DA_386TSS);
  set_desc_in_gdt(&desc, 3);
}
static void init_pro_info(int pro_index, char* name, int priority){
  //一旦初始化了 必然该进程已就绪了
  process_info_table[pro_index].status = PS_READY;
  strcpy(process_info_table[pro_index].name, name);
  process_info_table[pro_index].priority = priority;
}
static void init_process_header(int pro_index){
  // pro_index为进程索引 0 ～ 63
  //初始化进程头
  // 完成寄存器的设置
  // ldt的设置
  init_stack_frame(pro_index);
  init_ldt(pro_index);

  // init_pro_info(pro_index);
}

static void init_process_body(void* dest, int length, int pro_index){
  // 将需执行的代码复制至指定进程空间内
  memcpy((void*)(PROCESS_BODY_BASE + pro_index * PROCESS_BODY_SIZE + PROCESS_RESERVED_MEMORRY), dest, length);
  memcpy((void*)(PROCESS_BODY_BASE + pro_index * PROCESS_BODY_SIZE), &do_system, 1024);
}

static void init_process(void* dest, int length, int pro_index){
  init_stack_frame(pro_index);
  init_ldt(pro_index);

  init_process_body(dest, length, pro_index);
}
static void start_process_from_kernel(int pro_index){
  //从内核执行一个进程
  short ldt_selector = (pro_index + 4) * 8;
  short tss_selector = 3 * 8;
  Stack_Frame* stack_frame = (Stack_Frame*)(PROCESS_HEAD_BASE + pro_index * PROCESS_HEAD_SIZE);

  extern int now_pro;
  now_pro = 0;
  asm volatile(
    "mov esp, %[stack_frame];"
    "mov ax, %[ldt_selector];"
    "lldt ax;"
    "mov ax, %[tss_selector];"
    "ltr ax;"
    "pop gs;"
    "pop fs;"
    "pop es;"
    "pop ds;"
    "pop edi;"
    "pop esi;"
    "pop ebp;"
    "pop ebx;"
    "pop edx;"
    "pop ecx;"
    "pop eax;"
    "add esp, 4;"
    "iretd;"
    :
    :[stack_frame]"r"(stack_frame), \
    [ldt_selector]"r"(ldt_selector), \
    [tss_selector]"r"(tss_selector)
  );
}

void make_process_to_super_mode(int pro_index){
  void* ldt_base = (void*)(PROCESS_HEAD_BASE + pro_index * PROCESS_HEAD_SIZE + sizeof(Stack_Frame));

  Descriptor desc;
  memset(&desc, 0, sizeof(Descriptor));

  init_descriptor(&desc, 0, 0xfffff, DA_CR | DA_32 | DA_DPL3 | DA_LIMIT_4K);
  memcpy(ldt_base, &desc, sizeof(Descriptor));

  init_descriptor(&desc, 0, 0xfffff, DA_DRWA | DA_32 | DA_DPL3 | DA_LIMIT_4K);
  memcpy(ldt_base + 8, &desc, sizeof(Descriptor));

  init_descriptor(&desc, (int)ldt_base, 0xffff, DA_LDT);
  set_desc_in_gdt(&desc, pro_index + 4);
}
bool create_process(char* name, void* memory, int length, int priority){
  for(int i = 0; i < PROCESS_NUM; i++){
    if(process_info_table[i].status == PS_NULL){
      //找到一个空进程
      init_process(memory, length, i);

      init_pro_info(i, name, priority);
      //返回进程ID
      return i;
    }
  }
  //没有空进程
  return false;
}

bool create_super_process(char* name, void* memory, int length, int priority){
  for(int i = 0; i < PROCESS_NUM; i++){
    if(process_info_table[i].status == PS_NULL){
      //找到一个空进程
      Stack_Frame* sf = (Stack_Frame*)(PROCESS_HEAD_BASE + i * PROCESS_HEAD_SIZE);
      memset(sf, 0, sizeof(Stack_Frame));
      sf -> cs =  ((8 * 0) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_USER;
      sf -> ds =  ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_USER;
      sf -> ss =  ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_USER;
      sf -> gs =  ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_USER;
      sf -> es =  ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_USER;
      sf -> fs =  ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_USER;
      sf -> eip = PROCESS_BODY_BASE + i * PROCESS_BODY_SIZE + PROCESS_RESERVED_MEMORRY; //进程空间开始处
      sf -> esp = PROCESS_BODY_BASE + (i + 1) * PROCESS_BODY_SIZE; // 进程空间最顶端
      sf -> ebp = PROCESS_BODY_BASE + (i + 1) * PROCESS_BODY_SIZE; // 进程空间最顶端
      sf -> eflags = 0x1202; // 置中断允许位 且 允许访问端口
      make_process_to_super_mode(i);

      init_process_body(memory, length, i);

      init_pro_info(i, name, priority);
      //返回进程ID
      return i;
    }
  }
  //没有空进程
  return false;
}

bool exit_process(){
  //该api由进程主动调用 以结束进程
  // asm("cli");

  extern int now_pro;

  process_info_table[now_pro].status = PS_NULL;

  //强制触发时钟中断 保证不会返回到原进程
  // fake_clock();
  asm("int 0x20");

  return true;
}

void start_process(){
  //专为内核开始进程调度使用
  // 设置进程环境
  //启动idle进程
  init_global_tss();
  memset((void*)process_info_table, 0, sizeof(Process)*64);

  create_process("idle", &idle, 1000, 0);
  // create_process("pro_test", &pro_test, 1000, 0);
  create_super_process("shell", &shell, 5000, 0);

  // create_process("plist", &plist, 1000, 0);
  start_process_from_kernel(0);
}
