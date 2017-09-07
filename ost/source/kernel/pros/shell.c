 #include "../../include/kernel/pros.h"
#include "../../include/kernel/pros_in.h"

// ---------------------------------------------

// shell 的实现

// 该模块导出：
//   shell

// 该模块导入：
//   无



// -------------------------------------------
typedef struct command{
  char name[16];
  void* address;
} Command;

Command command_table[] = {
  {"test", &test},
  {"reboot", &reboot},
  {"shutdown", &shutdown},
  {"time", &time},
  {"tdsfme", &time},
  {"ok", &time},
};
int command_num = 6;

static int do_command(Command command_table[], int index);
static int command_filter(Command command_table[], int, char*);
static int search_command(Command command_table[], int, char*);
static void command_handler(char*);
static bool strcmp(char* dest, char* source);
static void strcpy(char* dest, char* source);

extern int do_system(int syscall_num, int one, int two, int three, int four);
int (* system)(int,...);
void shell(){
  system = &do_system;

  char read = -1;
  char* greet = "OST>>:";
  char color = make_color(BLACK, RED);

  while(1){
    puts_c(greet, color);

    char buffer[100] = {0}; // 存储输入的命令
    char* p_buffer = buffer;

    while((read = read_k()) != 10){
      if(read !=0 ){
        if(read == 40){
          //退格
          if(p_buffer - buffer > 0){
            p_buffer--;
            putchar(read);
          }
        }
        else{
          *p_buffer = read;
          if(p_buffer - buffer < 98)
            p_buffer++;
          putchar(read);
        }

      }
    }
    *p_buffer = 0;

    putchar(read);
    command_handler(buffer);
  }
}

static void command_handler(char* command){
  if(command_filter(command_table, command_num, command)){
    return;
  }
  else if(do_command(command_table, search_command(command_table, command_num, command))){
    putchar('\n');
  }
  else{
    printf("Wrong Command. Use \"/help\" to see all available commands.\n");
  }
}

static int do_command(Command command_table[], int index){
  if(index == -1)
    return 0;
  int pid = execute(command_table[index].name, command_table[index].address);
  while(wait(pid));
  return 1;
}
static int search_command(Command command_table[], int command_num, char* command){
  for(int i = 0; i < command_num; i++){
    if(strcmp(command, command_table[i].name)){
      return i;
    }
  }

  return -1;
}

static int command_filter(Command command_table[], int command_num, char* command){
  char* help = "/help";
  char* clear = "clear";
  if(strcmp(command, help)){
    printf("All commands list as follows:\n");
    for(int i = 0; i < command_num; i++){
      puts(command_table[i].name);
      putchar('\n');
    }
    return 1;
  }
  else if(strcmp(command, clear)){
    clear_s();
    return 2;
  }
  else{
    return 0;
  }
}

static bool strcmp(char* dest, char* source){
  while(*source != 0 || *dest != 0){
    if(*source != *dest)
      return false;
    source++;
    dest++;
  }

  return true;
}

static void strcpy(char* dest, char* source){
  while(*source != 0){
    *dest = *source;
    dest++;
    source++;
  }
  *dest = 0;
}
