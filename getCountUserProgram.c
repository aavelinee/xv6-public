#include "types.h"
#include "stat.h"
#include "user.h"


int main()
{
	int pid = getpid();
	int sys_num = 5;
	while (pid == 0)
		pid = getpid();
	pid = 1;
	get_count(pid, sys_num);
	exit();
}