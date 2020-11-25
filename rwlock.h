#ifndef _rwlock
#define _rwlock


struct rwlock {
	struct ticketlock rw_mutex;
	struct ticketlock mutex;
	int read_count;
};

struct rwlock rwl;

void initrwlock_func(struct rwlock* rwl);

void acquirereadlock(struct rwlock *rw);
void releasereadlock(struct rwlock *rw);

void acquirewritelock(struct rwlock *rw);
void releasewritelock(struct rwlock *rw);

#endif