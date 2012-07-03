#include "oslib.h"

//char *s[] = {"child", "a", "b", "cc", "d", 0};
char *s[] = {"child", 0};

int main(int argc, char *argv[])
{
//	printf("%s\n", argv[0]);
	printf("One more time! %d\n", argc);
	execv(1, s);
	return 0;
}

/*
int main(int argc, char *argv[])
{
	printf("Child!\n");
	printf("argc %d : %x\n", argc, (unsigned int)&argc);
	printf("argv %x\n", (unsigned int)argv);
	int i;
	for (i = 0; i < argc; ++ i)
		printf("%d %x %s\n", i, (int)(argv[i]), argv[i]);
	return 0;
}
*/
