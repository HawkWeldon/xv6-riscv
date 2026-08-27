#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if (addr + n < addr)
      return -1;
    if (addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_getppid()
{
  return myproc()->parent->pid;
}

uint64
sys_square()
{
  int num;
  argint(0, &num);
  return num * num;
}

uint64
sys_get_child_count()
{
  return myproc()->no_of_children;
}

uint64
sys_get_process_child_count()
{
  int pid;
  argint(0, &pid);
  return pid_get_no_children(pid);
}

uint64
sys_nfork()
{
  int n; 
  argint(0, &n);
  
  uint64 pid_arr;
  argaddr(1, &pid_arr);

  for (int i = 0; i < n; i++)
  {
    int pid = kfork();
    if (pid < 0) return -1;
    if (pid == 0) return 0;
    copyout(myproc()->pagetable, pid_arr + i * sizeof(int), 0, (char*)&pid, sizeof(int));
  }
  
  //copyout(myproc()->pagetable, 0, pid_arr,(char*)pids, n*sizeof(int)); This is giving weird issue with the op, I am just sending out one value at a time.
  return n;
}

uint64
sys_print_syscalls()
{
  return syscall_printer();
}

uint64
sys_print_process_syscalls()
{
  int pid;
  argint(0, &pid);
  return syscall_printer_pid(pid);
}

uint64
sys_get_inode_num()
{
  int fd;
  argint(0, &fd);
  
  struct proc *p = myproc();

  if (fd < 0 || fd >= NOFILE) return -1;

  struct file *f = p->ofile[fd];

  if (!(f != 0 && f->type == FD_INODE && f->readable != 0)) return -1;

  return f->ip->inum;
}

uint64
sys_get_read_offset()
{
  int fd;
  argint(0, &fd);
  
  struct proc *p = myproc();

  if (fd < 0 || fd >= NOFILE) return -1;

  struct file *f = p->ofile[fd];

  if (!(f != 0 && f->type == FD_INODE && f->readable != 0)) return -1;

  return f->off;
}

uint64
sys_peek2()
{
  int fd;
  argint(0, &fd);
  if (fd < 0 || fd >= NOFILE) return -3; // Specific values are requested for FD_INODE and EOF so I am using -3 here.

  uint64 destination;
  argaddr(1, &destination);

  int bytes;
  argint(2, &bytes);

  struct proc *p = myproc();
  struct file *f = p->ofile[fd];
  if (!(f != 0 && f->type == FD_INODE && f->readable != 0)) return -1;

  int offset_cpy = f->off;
  int n = fileread(f, destination, bytes);
  f->off = offset_cpy;

  if (n == 0) return -2;
  else if(n < 0) return -3;
  else return 0;
}

uint64
sys_pte_valid()
{
  uint64 va;
  argaddr(0, &va);

  if (va >= MAXVA) panic("va out of adress space");

  pagetable_t pagetable = myproc()->pagetable;
  pte_t *pte;

  for (int level = 2; level > 0; level--) 
  {
    pte = &pagetable[PX(level, va)];
    if (*pte & PTE_V) pagetable = (pagetable_t)PTE2PA(*pte);
    else return 0;
  }

  pte = &pagetable[PX(0, va)];

  if(pte == 0 || !(*pte & PTE_V)) return 0;
  return 1;
}

uint64
sys_get_pteflags()
{
  uint64 va;
  argaddr(0, &va);

  pte_t *pte = walk(myproc()->pagetable, va, 0);

  if(pte == 0 || !(*pte & PTE_V)) 
  {
    printk("Invalid va.\n");
    return 0;
  }

  printk("VA: %p -> R:%d W:%d X:%d U:%d \n", (void*)va, (*pte & PTE_R) > 0, (*pte & PTE_W) > 0, (*pte & PTE_X) > 0, (*pte & PTE_U) > 0);
  
  return 0;
}

uint64
sys_va2pa()
{
  uint64 va;
  argaddr(0, &va);

  uint64 pa = walkaddr(myproc()->pagetable, va);
  if (!pa) return -1;

  pa = pa + va%PGSIZE;

  return pa;
}

uint64
sys_getvasize()
{
  int pid;
  argint(0, &pid);

  return kgetvasize(pid);
}