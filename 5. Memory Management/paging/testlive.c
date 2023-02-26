#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int 
main(int argc, char *argv[]){
  //we need only 1 parameter along with the command
  if(argc != 2){
    fprintf(2, "Usage: %s sys_call_num command\n", argv[0]);
    exit(1);
  }

  int n = atoi(argv[1]);

  if(n < 0){
    fprintf(2, "%s: testlive failed\n", argv[0]);
    exit(1);
  }

  for(int i = 0; i < n; i++){
    sbrk(4096);
  }

  //printf("passed: %d\n", atoi(argv[1]));

  while(1){}

  exit(0);
}
