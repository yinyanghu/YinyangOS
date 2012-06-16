#include "oslib.h"

/*
void hanoi(int n, int a, int b, int c)
{
	if (n == 1)
	{
		printf("from %d to %d\n", a, c);
		return;
	}
	hanoi(n - 1, a, c, b);
	printf("from %d to %d\n", a, c);
	hanoi(n - 1, b, a, c);
}
*/

int main(void)
{
	
//	hanoi(4, 1, 2, 3);
	printf("Hello Yinyanghu!\n");
	
	printf("First PID = %d\n", getpid());
	int global = fork();
	printf("PID @_@ = %d\n", global);
	int k = 213123;
	if (global == 0)
	{
		{
		printf("%d %d\n", global, getpid());
		printf("Child Process, PID = %d, %d, fork() = %d\n", getpid(), k, global);
		printf("fork() = %d, Child Process, PID = %d, %d\n", global, getpid(), k);
		}
	}
	else
	{
		{
		printf("%d %d\n", global, getpid());
		printf("Father Process, PID = %d, %d, fork() = %d\n", getpid(), k, global);
		printf("fork() = %d, Father Process, PID = %d, %d\n", global, getpid(), k);
		}
	}
	printf("Exiting........\n");
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
