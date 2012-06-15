#include "oslib.h"
/*
int main(void)
{
	int k = fork();	
	if (k != 0)
	{
			while (1)
			printf("JAVA\n");
	}

	return 0;
}
*/

int main(void)
{
	while (1)
	{
		printf("PID = %d\n", getpid());
		int k = fork();
		if (k != 0)
		{
			break;
		}
	}
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
