#include "oslib.h"

//char *s[] = {"child", "a", "b", "cc", "d", 0};
char *s[] = {"child", 0};

int main(int argc, char *argv[])
{
	if (fork() == 0)
		execv(2, s);
	return 0;
}
