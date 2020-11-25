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
  // start///////////////////////
  system_calls[index_of_process_aln][index_of_syscall_ftm].name = "fork";
    system_calls[index_of_process_aln][index_of_syscall_ftm].counter++;


    int tcounter = system_calls[index_of_process_aln][index_of_syscall_ftm].time_index;
    cmostime(system_calls[index_of_process_aln][index_of_syscall_ftm].time[tcounter]);
    system_calls[index_of_process_aln][index_of_syscall_ftm].time_index ++;

  // end//////////////////////////////
  return fork();

}

int
sys_exit(void)
{
  // start///////////////////////
  system_calls[index_of_process_aln][index_of_syscall_ftm].name = "exit";
  system_calls[index_of_process_aln][index_of_syscall_ftm].counter++;

  int tcounter = system_calls[index_of_process_aln][index_of_syscall_ftm].time_index;
  cmostime(system_calls[index_of_process_aln][index_of_syscall_ftm].time[tcounter]);
  system_calls[index_of_process_aln][index_of_syscall_ftm].time_index ++;
  
  // end//////////////////////////////
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  // start///////////////////////
  system_calls[index_of_process_aln][index_of_syscall_ftm].name = "wait";
  system_calls[index_of_process_aln][index_of_syscall_ftm].counter++;

  int tcounter = system_calls[index_of_process_aln][index_of_syscall_ftm].time_index;
    cmostime(system_calls[index_of_process_aln][index_of_syscall_ftm].time[tcounter]);
    system_calls[index_of_process_aln][index_of_syscall_ftm].time_index ++;
  
  // end//////////////////////////////
  return wait();
}

int
sys_kill(void)
{
  // start///////////////////////
  system_calls[index_of_process_aln][index_of_syscall_ftm].name = "kill";
  system_calls[index_of_process_aln][index_of_syscall_ftm].counter++;
  

  int tcounter = system_calls[index_of_process_aln][index_of_syscall_ftm].time_index;
    cmostime(system_calls[index_of_process_aln][index_of_syscall_ftm].time[tcounter]);
    system_calls[index_of_process_aln][index_of_syscall_ftm].time_index ++;
  // end//////////////////////////////
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  // start//////////////////////
  system_calls[index_of_process_aln][index_of_syscall_ftm].arguments[tcounter][0]->type = "int";
  system_calls[index_of_process_aln][index_of_syscall_ftm].arguments[tcounter][0]->int_value = pid;


  // end///////////////////////
  return kill(pid);
}

int
sys_getpid(void)
{
  // start//////////////////////////////////
  system_calls[index_of_process_aln][index_of_syscall_ftm].name = "getpid";
  int tcounter = system_calls[index_of_process_aln][index_of_syscall_ftm].time_index;
    cmostime(system_calls[index_of_process_aln][index_of_syscall_ftm].time[tcounter]);
    system_calls[index_of_process_aln][index_of_syscall_ftm].time_index ++;


  
  // end////////////////////////////////////////
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;
  // start//////////////////////////////////
  system_calls[index_of_process_aln][index_of_syscall_ftm].name = "sbrk";
    system_calls[index_of_process_aln][index_of_syscall_ftm].counter++;

    int tcounter = system_calls[index_of_process_aln][index_of_syscall_ftm].time_index;
    cmostime(system_calls[index_of_process_aln][index_of_syscall_ftm].time[tcounter]);
    system_calls[index_of_process_aln][index_of_syscall_ftm].time_index ++;
  
  // end////////////////////////////////////////
  if(argint(0, &n) < 0)
    return -1;
  // start////////////////////////////////////////
  system_calls[index_of_process_aln][index_of_syscall_ftm].arguments[tcounter][0]->type = "int";
  system_calls[index_of_process_aln][index_of_syscall_ftm].arguments[tcounter][0]->int_value = n;
  // cprintf("sbrk n : %d\n", system_calls[index_of_process_aln][index_of_syscall_ftm].arguments[time_index_fa][0]->int_value);
  // end////////////////////////////////////////

  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  // start//////////////////////////////////
  system_calls[index_of_process_aln][index_of_syscall_ftm].name = "sleep";
    system_calls[index_of_process_aln][index_of_syscall_ftm].counter++;

    int tcounter = system_calls[index_of_process_aln][index_of_syscall_ftm].time_index;
    cmostime(system_calls[index_of_process_aln][index_of_syscall_ftm].time[tcounter]);
    system_calls[index_of_process_aln][index_of_syscall_ftm].time_index ++;
  
  // end////////////////////////////////////////
  if(argint(0, &n) < 0)
    return -1;
  // start////////////////////////////////////////
  system_calls[index_of_process_aln][index_of_syscall_ftm].arguments[tcounter][0]->type = "int";
  system_calls[index_of_process_aln][index_of_syscall_ftm].arguments[tcounter][0]->int_value = n;
  // end////////////////////////////////////////


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

  // start//////////////////////////////////
  system_calls[index_of_process_aln][index_of_syscall_ftm].name = "uptime";
    system_calls[index_of_process_aln][index_of_syscall_ftm].counter++;

    int tcounter = system_calls[index_of_process_aln][index_of_syscall_ftm].time_index;
    cmostime(system_calls[index_of_process_aln][index_of_syscall_ftm].time[tcounter]);
    system_calls[index_of_process_aln][index_of_syscall_ftm].time_index ++;
  
  // end////////////////////////////////////////
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

//Start///////////////////////////////////////////////////////////////////////////////////////////

int
sys_invoked_syscalls(void)
{
  int pid;
  system_calls[index_of_process_aln][index_of_syscall_ftm].name = "invoked_syscalls";
  // cprintf("name::::: %s\n", system_calls[index_of_process_aln][index_of_syscall_ftm].name);
    system_calls[index_of_process_aln][index_of_syscall_ftm].counter++;

    int tcounter = system_calls[index_of_process_aln][index_of_syscall_ftm].time_index;
    cmostime(system_calls[index_of_process_aln][index_of_syscall_ftm].time[tcounter]);
    system_calls[index_of_process_aln][index_of_syscall_ftm].time_index ++;

  if(argint(0, &pid) < 0)
    return -1;
  // start////////////////////////////////////////
  system_calls[index_of_process_aln][index_of_syscall_ftm].arguments[tcounter][0]->type = "int";
  system_calls[index_of_process_aln][index_of_syscall_ftm].arguments[tcounter][0]->int_value = pid;
  // end/////////////////////////////////////////  
  invoked_syscalls(pid);
  return 0;
}


int
sys_get_count(void)
{
  int pid;
  
  system_calls[index_of_process_aln][index_of_syscall_ftm].name = "get_count";
    system_calls[index_of_process_aln][index_of_syscall_ftm].counter++;

    int tcounter = system_calls[index_of_process_aln][index_of_syscall_ftm].time_index;
    cmostime(system_calls[index_of_process_aln][index_of_syscall_ftm].time[tcounter]);
    system_calls[index_of_process_aln][index_of_syscall_ftm].time_index ++;
  //+++++++++++++++++++++++++++++++++++++
  int sys_num = 11;
  //+++++++++++++++++++++++++++++++++++++
  if(argint(0, &pid) < 0)
    return -1;

  // start////////////////////////////////////////
  system_calls[index_of_process_aln][index_of_syscall_ftm].arguments[tcounter][0]->type = "int";
  system_calls[index_of_process_aln][index_of_syscall_ftm].arguments[tcounter][0]->int_value = pid;
  system_calls[index_of_process_aln][index_of_syscall_ftm].arguments[tcounter][1]->type = "int";
  system_calls[index_of_process_aln][index_of_syscall_ftm].arguments[tcounter][1]->int_value = sys_num;

  // end///////////////////////////////////////

  get_count(pid, sys_num);
  return 0;

}

int
sys_sort_syscalls(void)
{
  int pid;
  system_calls[index_of_process_aln][index_of_syscall_ftm].name = "sort_syscalls";
    system_calls[index_of_process_aln][index_of_syscall_ftm].counter++;

    int tcounter = system_calls[index_of_process_aln][index_of_syscall_ftm].time_index;
    cmostime(system_calls[index_of_process_aln][index_of_syscall_ftm].time[tcounter]);
    system_calls[index_of_process_aln][index_of_syscall_ftm].time_index ++;

  if(argint(0, &pid) < 0)
    return -1;

  system_calls[index_of_process_aln][index_of_syscall_ftm].arguments[tcounter][0]->type = "int";
  system_calls[index_of_process_aln][index_of_syscall_ftm].arguments[tcounter][0]->int_value = pid;

  sort_syscalls(pid);
  return 0;
}

int
sys_log_syscalls(void)
{
  system_calls[index_of_process_aln][index_of_syscall_ftm].name = "log_syscalls";
    system_calls[index_of_process_aln][index_of_syscall_ftm].counter++;

    int tcounter = system_calls[index_of_process_aln][index_of_syscall_ftm].time_index;
    cmostime(system_calls[index_of_process_aln][index_of_syscall_ftm].time[tcounter]);
    system_calls[index_of_process_aln][index_of_syscall_ftm].time_index ++;
  log_syscalls();
  return 0;
}

int
sys_inc_num(void)
{
  int num;
  system_calls[index_of_process_aln][index_of_syscall_ftm].name = "inc_num";
    system_calls[index_of_process_aln][index_of_syscall_ftm].counter++;

    int tcounter = system_calls[index_of_process_aln][index_of_syscall_ftm].time_index;
    cmostime(system_calls[index_of_process_aln][index_of_syscall_ftm].time[tcounter]);
    system_calls[index_of_process_aln][index_of_syscall_ftm].time_index ++;

  if(argint(0, &num) < 0)
    return -1;

  system_calls[index_of_process_aln][index_of_syscall_ftm].arguments[tcounter][0]->type = "int";
  system_calls[index_of_process_aln][index_of_syscall_ftm].arguments[tcounter][0]->int_value = num;

  inc_num(num);
  return 0;
}
//End///////////////////////////////////////////////////////////////////////////////////////////

