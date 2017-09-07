#include "../../include/kernel/pros.h"
#include "../../include/kernel/pros_in.h"

void test(){
  set_system;
  char data[1024];
  printf("%d %d %d\n", data[3], data[4], data[5]);
  read_floppy(0, p(data));
  printf("%d %d %d\n", data[3], data[4], data[5]);

  printf("hello world");
  exit();
}
