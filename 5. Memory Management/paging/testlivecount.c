#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int 
main(int argc, char *argv[]){
  //we need only 1 parameter along with the command
  if(argc != 1){
    fprintf(2, "Usage: %s sys_call_num command\n", argv[0]);
    exit(1);
  }

  int lc = livecount();

  if(lc < 0){
    fprintf(2, "%s: testlivecount failed\n", argv[0]);
    exit(1);
  }

  printf("Number of live pages being used: %d\n", lc);

  exit(0);
}
