#include "types.h"
#include "stat.h"
#include "user.h"
#include "ticketlock.h"


int dec_to_binary(uint pattern, int binary_pattern [])
{
  int arr[200];
  int i = 0;
  while(pattern > 0)
  {
    arr[i] = pattern%2;
    pattern /= 2;
    i++;
  }
  i--;
  int j=0;
  while(i >= 0)
  {
    binary_pattern[j] = arr[i];
    j++;
    i--;
  }
  return ++j;

}


int main()
{
	int pattern = 10;
	int binary[200];
	int pattern_size;
	int pid;

	pattern_size = dec_to_binary(pattern, binary);

	rwinit();
  	
  	pid = fork();
	
	for(int i =1; i < pattern_size ; i++)
		if(pid > 0)
		{
  			pid = fork();
  			if(pid == 0)
				rwtest(binary[i]);
		}
	for(int i=0; i< pattern_size; i++)
    	wait();
	exit();
}