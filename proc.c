#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

///////////////////////////////////////////////////////////start
// int creation_time_list[128];
// int time_index = 0;

int queue1_size;
int queue2_size;
int queue3_size;
int counter = 1;
// int lotteries;
///////////////////////////////////////////////////////////end


static struct proc *initproc;

int nextpid = 1;
int nextxaxis = 0;

extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

void
pinit(void)
{
  initlock(&ptable.lock, "ptable");
}

// Must be called with interrupts disabled
int
cpuid() {
  return mycpu()-cpus;
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu*
mycpu(void)
{
  int apicid, i;
  
  if(readeflags()&FL_IF)
    panic("mycpu called with interrupts enabled\n");
  
  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i) {
    if (cpus[i].apicid == apicid)
      return &cpus[i];
  }
  panic("unknown apicid\n");
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc*
myproc(void) 
{
  struct cpu *c;
  struct proc *p;
  pushcli();
  c = mycpu();
  p = c->proc;
  popcli();
  return p;
}

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.

//////////////////////////////////////////////////////////////////////start
//system call to give a process special priority
void
print_creation_time(void)
{
  struct proc *p;
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if(p->pid != 0)
      cprintf("%d\t" , p->xaxis);
  }
  cprintf("\n");
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if(p->pid != 0)
      cprintf("%d\t" , p->creation_time);
  }
}
void
set_priority(int pid,int number)
{
  acquire(&ptable.lock);
  struct proc *p;
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if(p->pid == pid)
    {
      p->priority = number;
      // cprintf("----------------------in the set_priority with pid : %d priority: %d\n",p->pid , p->priority);
    }
  }
  release(&ptable.lock);
}

void
set_lottery(int pid, int ticket_count)
{
  acquire(&ptable.lock);
  struct proc* p;
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if(p->pid == pid)
      break;
  }
  p->lottery_count = ticket_count;
  // cprintf("------------------------in the set_lottery with pid : %d lottery: %d\n",p->pid , p->lottery_count);
  release(&ptable.lock);
}

void
set_queue_level(int pid, int level)
{
  acquire(&ptable.lock);
  struct proc* p;
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if(p->pid == pid)
    {
      if(p->queue_level == 1)
      {
        queue1_size--;
        // cprintf("process %d exit from queue %d\n" , pid , p->queue_level);
      }
      else if(p->queue_level == 2)
        queue2_size--;
      else if(p->queue_level == 3)
        queue3_size--;
      if(level == 1)
        queue1_size++;
      else if(level == 2)
        queue2_size++;
      else if(level == 3)
        queue3_size++;
      p->queue_level = level;
      // cprintf("pid %d in the set_queue_level : level : %d\n", p->pid, p->queue_level);
      break;
    }
  }
  release(&ptable.lock);
}
int
get_queue_level(int pid)
{
  struct proc* p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if(p->pid == pid)
      return p->queue_level;
  }
  return -1;
}

void
process_info(void)
{
  struct proc* p;
  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  char *state;
  cprintf("name\tpid\tstate\tqueue level\tpriority\tlottery count\tcreation time\n");
  cprintf("--------------------------------------------------------------------------------------------\n");
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    state = states[p->state];
    if(p->pid != 0)
    cprintf("%s\t %d\t%s\t    %d\t\t   %d  \t\t%d\t\t   %d\n",p->name , p->pid , state , p->queue_level , p->priority
      , p->lottery_count , p->creation_time);

  }

}
//////////////////////////////////////////////////////////////////////end

static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;


found:
  p->state = EMBRYO;
  p->pid = nextpid++;
  if(nextpid > 2)
    p->xaxis = nextxaxis++;

  ///////////////////////////////////////////////////////////////////////start
  p->priority = 20;
  counter++;

  p->queue_level = 1; //processes made by allocproc placed in 3rd queue
  queue1_size++;

  // cprintf("in the alloc : %d\n", p->pid);

  p->lottery_count = 10;
  // creation_time_list[time_index] = ticks;
  // time_index ++;
  p->creation_time = ticks + p->pid; //sys_uptime();
  // for(int i = 0; i < 128; i++)
  // {

  // }

  //////////////////////////////////////////////////////////////////////end

  release(&ptable.lock);

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;

  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe*)sp;

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  return p;
}

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

  p = allocproc();
  
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  // this assignment to p->state lets other cores
  // run this process. the acquire forces the above
  // writes to be visible, and the lock is also needed
  // because the assignment might not be atomic.
  acquire(&ptable.lock);

  p->state = RUNNABLE;

  release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint sz;
  struct proc *curproc = myproc();

  sz = curproc->sz;
  if(n > 0){
    if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;
  switchuvm(curproc);
  return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *curproc = myproc();

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
  }

  // Copy process state from proc.
  if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }
  np->sz = curproc->sz;
  np->parent = curproc;
  *np->tf = *curproc->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  pid = np->pid;

  acquire(&ptable.lock);

  np->state = RUNNABLE;

  release(&ptable.lock);

  return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
  struct proc *curproc = myproc();
  struct proc *p;
  //////////////////////////////////////////////////
  struct proc *t;
  /////////////////////////////////////////////////
  int fd;

  if(curproc == initproc)
    panic("init exiting");

  ////////////////////////////////////////////////////////////////start
  if(curproc->queue_level == 1)
  {
    for(t = ptable.proc; t < &ptable.proc[NPROC]; t++)
    {
      if(t->queue_level == 1)
      {
        if(t->lottery_start > curproc->lottery_end)
        {
          t->lottery_start -= curproc->lottery_count;
          t->lottery_end -= curproc->lottery_count;
        }
      }
    }
  }

  // lotteries -= curproc->lottery_count;

  if(curproc->queue_level == 1)
    queue1_size--;
  else if(curproc->queue_level == 2)
    queue2_size--;
  else if(curproc->queue_level == 3)
    queue3_size--;

  // cprintf("before exit, priority: %d  pid: %d\n", curproc->priority, curproc->pid);
  ////////////////////////////////////////////////////////////////end

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd]){
      fileclose(curproc->ofile[fd]);
      curproc->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(curproc->cwd);
  end_op();
  curproc->cwd = 0;

  acquire(&ptable.lock);

  // Parent might be sleeping in wait().
  wakeup1(curproc->parent);

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == curproc){
      p->parent = initproc;
      if(p->state == ZOMBIE)
        wakeup1(initproc);
    }
  }

  // Jump into the scheduler, never to return.
  curproc->state = ZOMBIE;

  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
  struct proc *p;
  int havekids, pid;
  struct proc *curproc = myproc();
  
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != curproc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || curproc->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
  }
}

//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.


///////////////////////////////////////////////////////////////////start
int
minimum_priority()
{
  // acquire(&ptable.lock);

  int min_priority = 9999999;
  struct proc *p;
  struct proc *min_proc;
  min_proc = ptable.proc;
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if(p->state == RUNNABLE && p->queue_level == 3)
    {
      if(p->priority < min_priority)
      {
        min_priority = p->priority;
        min_proc = p;
      }
      else if(p->priority == min_priority)
      {
        if(p->creation_time < min_proc->creation_time)
        {
          min_priority = p->priority;
          min_proc = p;
        }
      }
    }
  }

  // release(&ptable.lock);

  return min_priority;
}

int
minimum_creation_time()
{
  int min_time = 99999999;
  struct proc *p;
  struct proc *min_proc;
  min_proc = ptable.proc;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if(p->state == RUNNABLE && p->queue_level == 2)
    {
      if(p->creation_time < min_time)
      {
        min_time = p->creation_time;
        min_proc = p;
      }
      else if(p->creation_time == min_time)
      {
        if(p->pid < min_proc->pid)
        {
          min_time = p->priority;
          min_proc = p;
        }
      }
    }
  }
  return min_time;
}

int rand()
{
  static int next = 3251 ; // Anything you like here - but not
                           // 0000, 0100, 2500, 3792, 7600,
                           // 0540, 2916, 5030 or 3009.
  next = ((next * next) / 100 ) % 10000 ;
  return next ;
}

int 
randInRange(int min, int max)
{
  return rand() % (max+1-min) + min ; 
}

////////////////////////////////////////////////////////////////////end

void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  int rand_number;
  c->proc = 0;
  
  for(;;){
    // Enable interrupts on this processor.
    sti();

    int is_in_first_queue = 0;
    int is_in_second_queue = 0;
    int sum_lotteries = 0;

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
//////////////////////////////////////////////////////////////////////////start
    if(queue1_size>0)
    {
      for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
      {
        if(p->state != RUNNABLE || p->queue_level != 1)
          continue;
        sum_lotteries += p->lottery_count;
      }
      rand_number = randInRange(0, sum_lotteries);
      for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
      {

        if(p->state != RUNNABLE || p->queue_level != 1)
          continue;
        else
        {
          // cprintf("rand : %d\n", rand_number);
          // cprintf("lottery_count :: %d && pid :: %d\n", p->lottery_count, p->pid);

          rand_number -= p->lottery_count;
          if(rand_number > 0)
          {
          //   cprintf("pid : %d - lottery : %d\n", p->pid, p->lottery_count);
          //   cprintf("rand : %d\n", rand_number);
            // cprintf("rand %d\n", rand_number);
            continue;
          }
          else
          {
            // cprintf("rand number :: %d\n", rand_number);


            c->proc = p;
            is_in_first_queue = 1;
            // Switch to chosen process.  It is the process's job
            // to release ptable.lock and then reacquire it
            // before jumping back to us.
            switchuvm(p);
            p->state = RUNNING;

            swtch(&(c->scheduler), p->context);
            switchkvm();

            //////////////////////////////////////////////////////////////////////start
            // cprintf("queue1_size : %d\n", queue1_size);
            // cprintf("queue 1 : ##cpu %d allocated to process with pid = %d\n" ,c->apicid, p->pid);
            //////////////////////////////////////////////////////////////////////end

            // Process is done running for now.
            // It should have changed its p->state before coming back.
            c->proc = 0;
            break;          
          }
        }
        
      }
    }

    if(queue2_size>0 && is_in_first_queue == 0)
    {

      // cprintf("*******in the second for : pid -> :\n");
      int min_creation_time = minimum_creation_time();
      for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
      {
        if(p->state != RUNNABLE || p->queue_level != 2)
        {
          continue;
        }
        else
        {
          if(p->creation_time != min_creation_time)
          {
            continue;
          }
        }

        // Switch to chosen process.  It is the process's job
        // to release ptable.lock and then reacquire it
        // before jumping back to us.
        c->proc = p;
        switchuvm(p);
        p->state = RUNNING;

        //////////////////////////////////////////////////////////////////////start
        // cprintf("queue 2 : ##cpu %d allocated to process with pid = %d\n" ,c->apicid, p->pid);
        // cprintf("queue 2 : queue_level : %d\n", p->queue_level);
        //////////////////////////////////////////////////////////////////////end

        swtch(&(c->scheduler), p->context);
        switchkvm();
        is_in_second_queue = 1;
        // Process is done running for now.
        // It should have changed its p->state before coming back.
        c->proc = 0;
        break;
      }
    }

    if(queue3_size>0 && is_in_first_queue == 0 && is_in_second_queue == 0)
    {
      int min_priority = minimum_priority();
      for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
      {
        if(p->state != RUNNABLE || p->queue_level != 3)
          continue;
        else
        {
          if(p->priority != min_priority)
          {
            continue;
          }
        }
        // Switch to chosen process.  It is the process's job
        // to release ptable.lock and then reacquire it
        // before jumping back to us.
        c->proc = p;
        switchuvm(p);
        p->state = RUNNING;

        //////////////////////////////////////////////////////////////////////start
        // cprintf("queue 3 : ##cpu %d allocated to process with pid = %d\n" ,c->apicid, p->pid);
        // cprintf("queue 3 : **************%d\n" ,p->priority);
        //////////////////////////////////////////////////////////////////////end

        swtch(&(c->scheduler), p->context);
        switchkvm();

        // Process is done running for now.
        // It should have changed its p->state before coming back.
        c->proc = 0;
        break;
      }
    }
/////////////////////////////////////////////////////////////////////////////end
    release(&ptable.lock);

  }
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{
  int intena;
  struct proc *p = myproc();

  if(!holding(&ptable.lock))
    panic("sched ptable.lock");
  if(mycpu()->ncli != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  swtch(&p->context, mycpu()->scheduler);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  acquire(&ptable.lock);  //DOC: yieldlock
  struct proc *p;
  p = myproc();
  p->state = RUNNABLE;
  //////////////////////////////////////////////////////////////start
  // if(p->queue_level == 1)
  //   queue1_size++;
  // else if(p->queue_level == 2)
  //   queue2_size++;
  // else if(p->queue_level == 3)
  //   queue3_size++;
  //////////////////////////////////////////////////////////////end
  sched();
  release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
  static int first = 1;
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);

  if (first) {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot
    // be run from main().
    first = 0;
    iinit(ROOTDEV);
    initlog(ROOTDEV);
  }

  // Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  if(p == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if(lk != &ptable.lock){  //DOC: sleeplock0
    acquire(&ptable.lock);  //DOC: sleeplock1
    release(lk);
  }
  // Go to sleep.
  // cprintf("in the sleep with pid : %d\n", p->pid);
  p->chan = chan;
  p->state = SLEEPING;
  // lotteries -= p->lottery_count;
  /////////////////////////////////////////////////////////////////////start
  // if(p->queue_level == 1)
  //   queue1_size--;
  // else if(p->queue_level == 2)
  //   queue2_size--;
  // else if(p->queue_level == 3)
  //   queue3_size--;
  /////////////////////////////////////////////////////////////////////end

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan)
    {
      // cprintf("in the wakeup1 with pid : %d\n", p->pid);
      p->state = RUNNABLE;
      // lotteries += p->lottery_count;
    }
  //////////////////////////////////////////////////////////////start
    // if(p->queue_level == 1)
    //   queue1_size++;
    // else if(p->queue_level == 2)
    //   queue2_size++;
    // else if(p->queue_level == 3)
    //   queue3_size++;
  //////////////////////////////////////////////////////////////end
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
      ////////////////////////////////////////////////////////////////////////////////////  
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  int i;
  struct proc *p;
  char *state;
  uint pc[10];

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    cprintf("%d %s %s", p->pid, state, p->name);
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}
