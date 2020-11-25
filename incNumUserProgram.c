#include "types.h"
#include "stat.h"
#include "user.h"

int main()
{
	int num = 531;
	int reg_temp = 0;
	asm("movl %%edx, %0" : "=r"(reg_temp));
	asm("movl %0, %%edx" :: "r"(num));
	inc_num(num);
	asm("movl %0, %%edx" :: "r"(reg_temp));
	exit();
}