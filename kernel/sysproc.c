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

