#include "oslib.h"

int main(void)
{
	
	printf("Second PID = %d\n", getpid());
	printf("Starting Sleeping....................\n");
	sleep(4);
	printf("Exiting........\n");
	exit(-128);
	return 0;
}



/*
int *c;

int main(void)
{
	c = (int *)0xC01E3704;
	while (1)
	{
		*c = (*c + 1) % 10;
	}
	return 0;
}
*/
