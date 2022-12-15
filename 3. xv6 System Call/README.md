# Changes made to xv6 files

## Makefile
```
UPROGS=\
  ... ...
  $U/_sh\
	$U/_stressfs\
	$U/_trace\        //added
  ... ...
```

## User/user.h
```
... ...
int sleep(int);
int uptime(void);
int trace(int);     //added
... ...
```

## User/usys.pl
```
... ...
entry("sleep");
entry("uptime");
entry("trace");    //added
```

## Kernel/proc.c
```
static struct proc*
allocproc(void)
{
... ...
  p->syscall_trace = -1;     //traces none by default

  return p;
}

... ...
int
fork(void)
{
... ...
  // Cause fork to return 0 in the child.
  np->trapframe->a0 = 0;
  np->syscall_trace = p->syscall_trace;
... ...
}
```

## Kernel/proc.h
```
// Per-process state
struct proc {
... ...
  char name[16];               // Process name (debugging)
  int syscall_trace;           // Syscall to trace
};
```

## Kernel/syscall.c
```
... ...
extern uint64 sys_close(void);
extern uint64 sys_trace(void);
... ...
static uint64 (*syscalls[])(void) = {
  ... ...
  [SYS_close]   sys_close,
  [SYS_trace]   sys_trace
};

char* syscallNames[] = {
  ... ...
  "close",
  "trace"
};

void
syscall(void)
{
  ... ...
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    p->trapframe->a0 = syscalls[num]();

    if (p->syscall_trace == num) {
      printf("pid: %d, syscall: %s, return value: %d\n", p->pid, syscallNames[num-1], p->trapframe->a0);
    }
  }
  ... ...
}
```

## Kernel/syscall.h
```
... ...
#define SYS_close  21
#define SYS_trace  22
```

## Kernel/sysproc.c
```
... ...
uint64
sys_trace(void)
{
  int n;
  argint(0, &n);
  struct proc *p = myproc();
  p->syscall_trace = n;
  return n;
}
... ...
```
