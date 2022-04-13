#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->vlimit;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// My memory protect function:
// Takes an address to start at, and a length of addresses
// to protect
// Returns 0 on success, and -1 on failure
int
sys_mprotect(void)
{
  pte_t *page_table;
  void *addr;
  int len;

  // Fetch system call argument for address
  if(argptr(0, &addr, 32) < 0)
    // Handle error condition with faulty argument given
    return -1;

  // Fetch first system call argument for length
  if(argint(0, &len) < 0)
    // Handle error condition with faulty argument given
    return -1;

  // Handle failure case when length <= 0
  if (len <= 0) {
    return -1;
  }

  // Get page table
  page_table = myproc()->pgdir;

  // Remove PTE_W from table
  *page_table &= ~PTE_W;

  // Successful execution, return 0
  return 0;
} 

// My memory unprotect function:
// Takes an address to start at, and a length of addresses
// to protect
// Returns 0 on success, and -1 on failure
int
sys_munprotect(void)
{
  pte_t *page_table;
  void *addr;
  int len;

  // Fetch system call argument for address
  if(argptr(0, &addr, 32) < 0)
    // Handle error condition with faulty argument given
    return -1;

  // Fetch first system call argument for length
  if(argint(0, &len) < 0)
    // Handle error condition with faulty argument given
    return -1;

  // Handle failure case when length <= 0
  if (len <= 0) {
    return -1;
  }

  // Get page table
  page_table = myproc()->pgdir;

  // Add PTE_W to all entries
  *page_table |= PTE_W;

  // Successful execution, return 0
  return 0;
} 