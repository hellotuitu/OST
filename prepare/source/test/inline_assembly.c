//test gcc inline assembly
//the command to compile: gcc inline_assembly.c -o inline_assembly
// in ubuntu, test passed.
#include<stdio.h>
int main(){
  int a = 1, b = 2, c = 0;
  //calculate c = a + b
  asm(
    "mov eax, %[params_one];"
    "add eax, %[params_two];"
    "mov %[output], eax;"
    : [output]"=g"(c)
    : [params_two]"g"(a), [params_one]"g"(b)
    : "memory"
  );
  printf("temp = %d", c);

  return 0;
}
