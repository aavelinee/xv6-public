#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
int main(int argc, char *argv[])
{
	int child_num = 0;
	int pid = 0;
	int tpid = 0;
	if(argc <= 1)
	{
		child_num = 1;
	}
	else
	{
		child_num = atoi(argv[1]);
		if(child_num < 0)
			child_num = 1;
		for(int i = 0 ; i < child_num; i ++)
		{
			pid = fork();
			if(pid < 0)
			{
				printf(1 , "falid in process generation\n");
			}
			else if(pid > 0)
			{
				tpid = getpid();
				printf(1 , "process parent %d generated process child %d\n", tpid , pid);
				wait();
			}
			else
			{
				tpid = getpid();
				printf(1 , "process child %d generated\n", tpid);
				// int temp = 1;
				// for(int j=1; j<2000000; j++)
				// {
				// 	temp = j + temp;
				// }
				double z, x;
				for ( z = 0; z < 8000000.0; z += 0.01 )
         			x =  x + 3.14 * 89.64;
         		break;
			}

		}
	}
	exit();

}