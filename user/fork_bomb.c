#include "oslib.h"

int	f[200];

int main(void)
{

	while (1)
	{
		printf("PID = %d\n", getpid());
		fork();
	}
		
	return 0;
}


