#include "types.h"
#include "stat.h"
#include "user.h"


int main()
{
	int pid = getpid();
	while (pid == 0)
		pid = getpid();
	log_syscalls();
	exit();
}