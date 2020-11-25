#include "types.h"
#include "stat.h"
#include "user.h"
#include "ticketlock.h"

#define NCHILD 10
int main()
{
	ticketlockinit();

	int pid;

  	pid = fork();

	for(int i =1; i < NCHILD; i++)
  		if(pid > 0)
  			pid = fork();

  	if(pid < 0)
    	printf(2, "fork errror\n");
  	else if(pid == 0)
  	{
  		printf(1 , "******************************************************\n");
    	printf(1,"child adding to shared counter________________________\n");
    	ticketlocktest();
  	}
  	else
  	{
    	for(int i=0; i<NCHILD; i++)
      		wait();
    	printf(1, "user program finished\n");
  	}
  	exit();
}