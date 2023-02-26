// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

#define MAXSWAPSTRUCTS 10000

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct sw_struct {
  int pid;
  uint64 va;
  struct swap *s;
};

struct sw_pages {
  int pid;
  uint va;
};

static int struct_curr = 0;
struct sw_struct swapped_structs[MAXPHYPAGES] ={0};

struct sw_pages live_pages[MAXPHYPAGES] ={0};
static int curr = 0; //list is empty

void
incrementCurrent()
{
  curr++;
}

void
decrementCurrent()
{
  curr--;
}

void
printLivePages()
{
  for(int i = 0; i < curr; i++){
    printf("Allocated page with pid: %d and va: %d\n", live_pages[i].pid, live_pages[i].va);
  }
}

void
insertFirst(int pid, uint64 va)
{
  // if(curr == MAXPHYPAGES-1) return;

  // for(int i = 0; i < MAXPHYPAGES-1; i++){
  //   live_pages[i+1] = live_pages[i];
  // }

  live_pages[curr].pid = pid;
  live_pages[curr].va = va; //increment where called

  //printLivePages();
}

struct sw_pages
deleteFirst()
{
  //if(curr == 0) return 0;

  struct sw_pages temp = live_pages[curr-1];
  return temp;
}

int
getLiveCount()
{
  // int s = 0;
  // for(int i = 0; i < MAXPHYPAGES; i++){
  //   if(live_pages[i].pid == 0) break;
  //   s++;
  // }

  // return s;
  return curr;
}

int
removeFromLivePage(int pid, uint64 va, int swapped)
{
  //printf("removeFromLivePage with pid: %d and va: %d\n", pid, va);
  //printLivePages();
  int i = -1, j;
  for(i = 0; i < MAXPHYPAGES-1; i++){
    //printf("searching for i...%d\n", i);
    if((live_pages[i].pid == pid) && (live_pages[i].va == va)){
      //printf("found the page to remove\n");
      break;
    }
  }

  //printf("i: %d\n", i);

  if(i == MAXPHYPAGES-1) return -1;

  for(j = i+1; j < MAXPHYPAGES; j++){
    live_pages[j-1].pid = live_pages[j].pid;
    live_pages[j-1].va = live_pages[j].va;
  }

  live_pages[MAXPHYPAGES-1].pid = 0;
  live_pages[MAXPHYPAGES-1].va = 0;

  //decrementCurrent();

  if(swapped){  
    printf("in swapped\n");
    struct swap* s = findStruct(pid, va);
    swapfree(s);
  }

  return 1;
}

void
swapOutLivePage()
{
  struct swap* s = swapalloc();
  struct sw_pages sp = deleteFirst();
  int pid = sp.pid;
  uint64 va = sp.va;
  // printf("doing swapout\n");
  struct proc *p = getProcess(pid);
  pagetable_t pt = p->pagetable;
  pte_t* pte = walk(pt, va, 0);
  *pte &= (~PTE_V);
  *pte |= PTE_SWAP;
  uint64 pa = PTE2PA(*pte);
  swapout(s, (char*)pa);
  insertStruct(pid, va, s);
  // printf("swapout and insert struct done\n");
  
  //kfree((void*)pa);
}

void swapInLivePage(int pid, uint64 va)
{
  //get char* mem for swapin
  struct swap* s = findStruct(pid, va);
  swapin((char*)va, s);
  swapfree(s);
  printf("swapin complete for pid %d and va %d\n", pid, va);
}

void
incrementCurrentStruct()
{
  struct_curr++;
}

void
decrementCurrentStruct()
{
  struct_curr--;
}

void 
insertStruct(int pid, int va, struct swap *s) {
  swapped_structs[struct_curr].pid = pid;
  swapped_structs[struct_curr].va = va;
  swapped_structs[struct_curr].s = s;

  // incrementCurrentStruct();
}

struct swap*
findStruct(int pid, uint64 va) {
  struct swap* temp = 0;
  int i = -1, j = -1;
  for(i = 0; i < MAXPHYPAGES-1; i++){
    if((swapped_structs[i].pid == pid) && (swapped_structs[i].va == va)){
      //decrementCurrentStruct();
      temp =  swapped_structs[i].s;
      break;
    }
  }

  if(i == MAXPHYPAGES-1) return 0;

  for(j = i+1; j < MAXPHYPAGES; j++){
    swapped_structs[j-1].pid = swapped_structs[j].pid;
    swapped_structs[j-1].va = swapped_structs[j].va;
    swapped_structs[j-1].s = swapped_structs[j].s;
  }

  swapped_structs[MAXPHYPAGES-1].pid = 0;
  swapped_structs[MAXPHYPAGES-1].va = 0;
  swapped_structs[MAXPHYPAGES-1].s = 0;

  decrementCurrentStruct();

  return temp;
}

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
