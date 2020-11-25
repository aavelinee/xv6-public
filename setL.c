#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
	if(argc > 3 || argc < 3)
		printf(1, "inappropriate arguments\n");
	else if(atoi(argv[1]) < 1 || atoi(argv[2]) < 1)
		printf(1, "inappropriate value for arguments\n");
	else
	{
		set_lottery(atoi(argv[1]) , atoi(argv[2]));
	}
	exit();


}