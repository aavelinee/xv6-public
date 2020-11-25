#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "ticketlock.h"

void
initticketlock_func(struct ticketlock* tl)
{
    tl->now_running = 0;
    tl->next_ticket = 0;
    cprintf("ticketlock successfully initialized for process: %d \n", myproc()->pid );
}

void
acquireticketlock(struct ticketlock* tl)
{
	pushcli();
	int my_ticket = fetch_and_add(&(tl->next_ticket),1);
	cprintf("ticket number for process %d is : %d " ,myproc()->pid , my_ticket);
	cprintf("now running : %d , next ticket : %d ,for process: %d \n", tl->now_running ,
            tl->next_ticket,myproc()->pid);
    cprintf("now_running : %d " ,tl->now_running);
    cprintf("my_ticket : %d " ,my_ticket)   ;

    while(tl->now_running != my_ticket) 
    {
    	ticketsleep(tl);
    }
    cprintf("acquire for process: %d \n" , myproc()->pid);
    popcli();
}

void
releaseticketlock(struct ticketlock* tl)
{
	pushcli();
    fetch_and_add(&(tl->now_running), 1);
    cprintf("releasse for process : %d\n", myproc()->pid);
    wakeup(tl);
    popcli();
}