#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "ticketlock.h"
#include "rwlock.h"

void initrwlock_func(struct rwlock *rw)
{
	rw->read_count = 0;
	initticketlock_func(&(rw->mutex));
	initticketlock_func(&(rw->rw_mutex));
    cprintf("rwlock successfully initialized for process: %d \n", myproc()->pid );
}

void acquirereadlock(struct rwlock *rw)
{
	acquireticketlock(&(rw->mutex));
	rw->read_count++;
	if ((rw->read_count) == 1)
		acquireticketlock(&(rw->rw_mutex));
	releaseticketlock(&(rw->mutex));
}
void releasereadlock(struct rwlock *rw)
{
	acquireticketlock(&(rw->mutex));
	rw->read_count--;
	if (rw->read_count == 0)
		releaseticketlock(&(rw->rw_mutex));
	releaseticketlock(&(rw->mutex));
}

void acquirewritelock(struct rwlock *rw)
{
	acquireticketlock(&(rw->rw_mutex));
}
void releasewritelock(struct rwlock *rw)
{
	releaseticketlock(&(rw->rw_mutex));
}