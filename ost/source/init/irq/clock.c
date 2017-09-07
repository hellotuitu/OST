#include "../../include/init/int_handler.h"
#include "../../include/kernel/process.h"

// ---------------------------------------------

// 时钟中断处理

// 该模块导出：
//   变量 ticks now_pro re_enter process_task_pointer process_ldt_selector
//   时钟中断处理函数 clock_handler

// 该模块导入：
//   无


// -------------------------------------------

#define TIMER0          0x40	/* I/O port for timer channel 0 */
#define TIMER_MODE      0x43	/* I/O port for timer mode control */
#define RATE_GENERATOR	0x34	/* 00-11-010-0 : Counter0 - LSB then MSB - rate generator - binary */
#define TIMER_FREQ	1193182L/* clock frequency for timer in PC and AT */
#define HZ		100	/* clock freq (software settable on IBM-PC) */


int ticks = 0;  // 时钟滴答计数 这样是不安全的 容易溢出
int now_pro = 0; // 目前的进程号
unsigned int re_enter = 0; // 中断重入

//初始化进程0的ldt选择子
volatile int process_task_pointer = PROCESS_HEAD_BASE;

//初始化为进程0的任务
volatile int process_ldt_selector = 32;

//调度函数 返回下一个应该执行的进程的pid
static int schedule();

//进程切换函数
static void switch_to(int);


static void switch_to(int pro_index){
  // switch_to 并不执行真正的切换 只是修改一些全局变量

  //当前进程与要切换进程一致 则直接返回
  if(pro_index == now_pro)
    return;

  //修改当前进程
  now_pro = pro_index;

  // ldt和gdt的设置
  process_ldt_selector = (pro_index + 4) * 8;
  process_task_pointer = (int)(PROCESS_HEAD_BASE + pro_index * PROCESS_HEAD_SIZE);

  // 任务结构的设置
  Tss* tss = (Tss*)(PROCESS_GLOBAL_TSS);
  tss -> esp0 = PROCESS_HEAD_BASE + pro_index * PROCESS_HEAD_SIZE + sizeof(Stack_Frame);
}

static int schedule(){
  // 得出下一个要执行的进程

  // 进程切换的思路
  // 采用轮转的方法
  // 若无其他进程可以切换 则执行idle进程

  //从当前进程往前数
  for(int i = now_pro + 1; i < PROCESS_NUM; i++){
    if(process_info_table[i].status == PS_RUNNING){
      return i;
    }
    else if(process_info_table[i].status == PS_READY){
      process_info_table[i].status = PS_RUNNING;
      return i;
    }
  }

  //前面没有 从头开始数
  for(int i = 1; i < now_pro; i++){
    if(process_info_table[i].status == PS_RUNNING){
      return i;
    }
    else if(process_info_table[i].status == PS_READY){
      process_info_table[i].status = PS_RUNNING;
      return i;
    }
  }
  //都没有 检查当前是否可以执行
  if(process_info_table[now_pro].status == PS_RUNNING)
    return now_pro;

  // 否则执行idle
  return 0;
}
void clock_handler(){
  // 目前的处理思路
  // 只有在非重入的情况下切换
  ticks++;

  // if(re_enter != 1){
  //   // 重入时不切换进程 直接返回
  //   // 这样的处理方式 会带来一些问题
  //   // 当一个进程循环调用系统调用时 在时钟频率不高的情况下 很可能无法进行进程切换
  //   return;
  // }
  // do_debug("test");
  int next_pro = schedule();

  switch_to(next_pro);
}

void force_switch_to(int next_pro){
  switch_to(next_pro);

  asm volatile(
    "mov esp, %[stack_frame];"
    "mov ax, %[ldt_selector];"
    "lldt ax;"
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
    :[stack_frame]"r"(process_task_pointer), \
    [ldt_selector]"r"((short)process_ldt_selector)
  );
}
void fake_clock(){
  // 有时在中断重入时必须切换进程
  int next_pro = schedule();

  force_switch_to(next_pro);
}

void set_clock_frequency(){
  //这个函数与时钟中断无关
  //函数用来设置时钟中断的频率
  outbyte(TIMER_MODE, RATE_GENERATOR);
	outbyte(TIMER0, (t_8) (TIMER_FREQ/HZ) );
	outbyte(TIMER0, (t_8) ((TIMER_FREQ/HZ) >> 8));
}
