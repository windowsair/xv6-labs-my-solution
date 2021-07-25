#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"


uint64
sys_sigalarm(void)
{
  int tick;
  uint64 handle_address;
  struct proc *p = myproc();

  if(argint(0, &tick) < 0 || argaddr(1, &handle_address) < 0)
    return -1;

  p->alarm_tick = tick;
  p->alarm_handle = (void (*)(void))handle_address;
  if (tick == 0 && handle_address == 0) {
    p->alarm_set = 0;
  }
  else {
    p->alarm_set = 1;
  }

  return 0;
}

uint64
sys_sigreturn(void)
{
  struct proc *p = myproc();
  if (p->alarm_running) {
    p->alarm_running = 0;
    // Recover user register
    memmove(p->trapframe, &p->pre_trapframe, sizeof(struct trapframe));
  }
  return 0;
}