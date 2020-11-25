#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
	if(argc != 1)
		printf(1, "inappropriate arguments\n");

	else
		process_info();
	exit();


}