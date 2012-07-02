#include "oslib.h"

char *s[] = {"child", "a", "b", "cc", "d", 0};

int main(void)
{
	printf("Father!\n");
	if (fork() == 0)
	{
		printf("Fork OK!\n");
		execv(2, s);
	}
	return 0;
}
