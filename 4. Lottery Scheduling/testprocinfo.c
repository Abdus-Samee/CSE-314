#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
#include "kernel/pstat.h"

int
main(int argc, char *argv[]){
    
    if(argc > 1){
        fprintf(2, "Usage: %s sys_call_num command\n", argv[0]);
        exit(1);
    }

    struct pstat p;

    if(getpinfo(&p) < 0){
        fprintf(2, "%s: getpinfo failed\n", argv[0]);
        exit(1);
    }

    printf("PID\t | In Use\t| Original Tickets\t | Current Tickets | Time Slices\n");
    for(int i = 0; i < NPROC; i++){
        if(p.inuse[i]){
            printf("%d\t | %d\t\t|%d\t\t\t   | %d\t\t\t | %d\n", p.pid[i], p.inuse[i], p.tickets_original[i], p.tickets_current[i], p.time_slices[i]);
        }
    }

    exit(0);
}
