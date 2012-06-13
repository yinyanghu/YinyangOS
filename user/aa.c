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
	int k = 10;
	while (k --)
	{
		printf("Hello Yinyanghu!\n");
	}
	
	printf("First PID = %d\n", getpid());
	printf("Waiting PID = %d\n", waitpid(getpid() + 1));
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
