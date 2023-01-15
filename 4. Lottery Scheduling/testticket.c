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

  if(settickets(atoi(argv[1])) < 0){
    fprintf(2, "%s: settickets failed\n", argv[0]);
    exit(1);
  }

  while(1){}

  exit(0);
}
