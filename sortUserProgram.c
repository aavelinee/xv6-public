#include "types.h"
#include "stat.h"
#include "user.h"


int main()
{
	int pid = getpid();
	while (pid == 0)
		pid = getpid();
	// int pid = -1;
	sort_syscalls(pid);
	exit();
}