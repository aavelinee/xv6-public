#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "syscall.h"
#include "date.h"


// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

// Fetch the int at addr from the current process.
int
fetchint(uint addr, int *ip)
{
  struct proc *curproc = myproc();

  if(addr >= curproc->sz || addr+4 > curproc->sz)
    return -1;
  *ip = *(int*)(addr);
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int
fetchstr(uint addr, char **pp)
{
  char *s, *ep;
  struct proc *curproc = myproc();

  if(addr >= curproc->sz)
    return -1;
  *pp = (char*)addr;
  ep = (char*)curproc->sz;
  for(s = *pp; s < ep; s++){
    if(*s == 0)
      return s - *pp;
  }
  return -1;
}

// Fetch the nth 32-bit system call argument.
int
argint(int n, int *ip)
{
  return fetchint((myproc()->tf->esp) + 4 + 4*n, ip);
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size bytes.  Check that the pointer
// lies within the process address space.
int
argptr(int n, char **pp, int size)
{
  int i;
  struct proc *curproc = myproc();
 
  if(argint(n, &i) < 0)
    return -1;
  if(size < 0 || (uint)i >= curproc->sz || (uint)i+size > curproc->sz)
    return -1;
  *pp = (char*)i;
  return i;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int
argstr(int n, char **pp)
{
  int addr;
  if(argint(n, &addr) < 0)
    return -1;
  return fetchstr(addr, pp);
}

extern int sys_chdir(void);
extern int sys_close(void);
extern int sys_dup(void);
extern int sys_exec(void);
extern int sys_exit(void);
extern int sys_fork(void);
extern int sys_fstat(void);
extern int sys_getpid(void);
extern int sys_kill(void);
extern int sys_link(void);
extern int sys_mkdir(void);
extern int sys_mknod(void);
extern int sys_open(void);
extern int sys_pipe(void);
extern int sys_read(void);
extern int sys_sbrk(void);
extern int sys_sleep(void);
extern int sys_unlink(void);
extern int sys_wait(void);
extern int sys_write(void);
extern int sys_uptime(void);

//Start//////////////////////////////////////////////////////////////////////////////////////////////
extern int sys_sort_syscalls(void);
extern int sys_invoked_syscalls(void);
extern int sys_get_count(void);
extern int sys_log_syscalls(void);
extern int sys_inc_num(void);
//End////////////////////////////////////////////////////////////////////////////////////////////////

static int (*syscalls[])(void) = {
[SYS_fork]    sys_fork,
[SYS_exit]    sys_exit,
[SYS_wait]    sys_wait,
[SYS_pipe]    sys_pipe,
[SYS_read]    sys_read,
[SYS_kill]    sys_kill,
[SYS_exec]    sys_exec,
[SYS_fstat]   sys_fstat,
[SYS_chdir]   sys_chdir,
[SYS_dup]     sys_dup,
[SYS_getpid]  sys_getpid,
[SYS_sbrk]    sys_sbrk,
[SYS_sleep]   sys_sleep,
[SYS_uptime]  sys_uptime,
[SYS_open]    sys_open,
[SYS_write]   sys_write,
[SYS_mknod]   sys_mknod,
[SYS_unlink]  sys_unlink,
[SYS_link]    sys_link,
[SYS_mkdir]   sys_mkdir,
[SYS_close]   sys_close,
//STart ////////////////////////////////////////////////////////////////////////
[SYS_sort_syscalls]   sys_sort_syscalls,
[SYS_invoked_syscalls]   sys_invoked_syscalls,
[SYS_get_count]   sys_get_count,
[SYS_log_syscalls]   sys_log_syscalls,
[SYS_inc_num]    sys_inc_num,
// End ////////////////////////////////////////////////////////////////////////
};


// start /////////////////////////

int getintSize(int number)
{
    int counter = 0;
    int tempNumber = 0;
    tempNumber = number;
    while(tempNumber != 0)
    {
        tempNumber = tempNumber / 10;
        counter ++;
    }
    return counter;
}

void convertIntToString(char out[], int num)
{
    int count=0;
    int temp;
    temp = num;
    if (num == 0)
    {
        out[0] = '0';
        out[1] = '\0';
        return;
    }
    while(temp > 0)
    {
        temp /= 10;
        count++;
    }
    for(int i =0; i<count; i++)
      out[i] = '\0';
   
    int i = count-1;
    
    while(num > 0)
    {   
        out[i] = ((num % 10) + '0');
        num /= 10;
        i--;
    }
    out[count] = '\0';
}

void
syscall(void)
{
  int num;
  int index = 0;
  int isexists = 0;
  int time_index;
  struct proc *curproc = myproc();  //returns the process that the syscall is in it : current process
                                    //no new process is generated
  num = curproc->tf->eax;
  index_of_syscall_ftm = num;

//Start/////////////////////////////////////////////////////////////////////////
  for(int i=0; i<NPROC; i++)
  {
    if(system_calls[i][0].pid == curproc->pid)
    {
      index = i;
      isexists = 1;
      break;
    }
  }

  if(isexists == 0)
  {
    for(int i=0; i<NPROC; i++)
    {
      if(system_calls[i][0].pid == 0)
      {
        for(int j=0; j<NSYS; j++)
          system_calls[i][j].pid = curproc->pid;
        index = i;
        break;
      }
    }
  }

  index_of_process_aln = index;
  if(num > 0 && num < NELEM(syscalls) && syscalls[num])
  {
    time_index = system_calls[index][num].counter;
    system_calls[index][num].number = num;

    curproc->tf->eax = syscalls[num]();

    total_counter ++;

    for(int i=0; i<NLOG; i++)
      if(log_info[total_counter].is_killed == 0)
      {
        
        log_info[total_counter].pid = curproc->pid;
        log_info[total_counter].name = system_calls[index][num].name;
        log_info[total_counter].is_killed = curproc->killed;
        log_info[total_counter].time->second = system_calls[index][num].time[time_index]->second;
        log_info[total_counter].time->minute = system_calls[index][num].time[time_index]->minute;
        log_info[total_counter].time->hour = system_calls[index][num].time[time_index]->hour;
        log_info[total_counter].time->day = system_calls[index][num].time[time_index]->day;
        log_info[total_counter].time->month = system_calls[index][num].time[time_index]->month;
        log_info[total_counter].time->year = system_calls[index][num].time[time_index]->year;    
      }
//End///////////////////////////////////////////////////////////////////////////

  } else {
    cprintf("%d %s: unknown sys call %d\n",
            curproc->pid, curproc->name, num);
    curproc->tf->eax = -1;
  }
}
