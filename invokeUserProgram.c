#include "types.h"
#include "stat.h"
#include "user.h"


int main()
{
	// int pid = getpid();
	// while (pid == 0)
	// 	pid = getpid();
	int pid = 2;
	invoked_syscalls(pid);
	// pid = getpid();
	// while (pid == 0)
	// 	pid = getpid();
	// sort_syscalls(pid);
	// int chpid = fork();
	// if(chpid == 0)
	// {
	// 	log_syscalls();
	// }
	// else
	// {
	// 	invoked_syscalls(pid);
	// 	wait();
	// 	invoked_syscalls(pid);
	// }
	
	exit();
}