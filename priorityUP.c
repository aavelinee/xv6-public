#include "types.h"
#include "stat.h"
#include "user.h"

int main()
{
	int pid = getpid();
	// int temp;
	set_lottery(getpid(), 100000);
	for(int i=0; i<10; i++)
	{
		if(pid > 0)
		{
			pid = fork();
			if(pid == 0)
			{
				break;
			}
			if(pid > 0) {
				// if(pid %2 != 0)
				// 	set_lottery(pid, 1000 + pid);
				// //dast be test priority nazan!!
				// if(pid % 2 != 0)
				// {	
				// 	set_priority(pid , pid);
				// 	printf(1,"child %d with lottery %d\n", pid, getpid());
				// }
				// if(pid % 2 != 0)
				{	

					// print_creation_time();
					// process_info();
					// set_queue_level(pid , 3);
					// process_info();

					// print_creation_time();

					// printf(1,"child %d with lottery %d\n", pid, getpid());
				}
			}
		}
	}

	if(pid == 0)
	{
		int temp = 1;
		for(int j=1; j<2000000; j++)
		{
			temp = j + temp;
		}
		printf(1, "%d\n", temp);
		exit();
	}
	process_info();
	// print_creation_time();
	if(pid > 0)
	{
		process_info();
		set_queue_level(10 , 1);
		process_info();


	}
	for(int i=0; i<30; i++)
		if(pid > 0)
			wait();
	// printf(1, "parent done\n");
	// printf(1, "+++++++++++++\n");


	exit();
}