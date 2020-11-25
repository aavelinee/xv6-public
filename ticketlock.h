#ifndef _ticketlock
#define _ticketlock



struct ticketlock {
	int now_running;
	int next_ticket;
};

struct ticketlock tl;

void initticketlock_func(struct ticketlock* tl);
void acquireticketlock(struct ticketlock* tl);
void releaseticketlock(struct ticketlock* tl);

#endif